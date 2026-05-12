#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

struct FArguments {
    std::string ModuleName;
    fs::path ModuleRoot;
    fs::path PublicRoot;
    fs::path PrivateRoot;
    fs::path OutputPublicRoot;
    fs::path OutputPrivateRoot;
    std::string RegistrationFunction;
};

struct FToken {
    std::string Text;
    std::size_t Offset = 0;
    std::size_t Line = 0;

    [[nodiscard]] bool IsIdentifier() const {
        if (Text.empty()) {
            return false;
        }

        if (!(std::isalpha(static_cast<unsigned char>(Text.front())) || Text.front() == '_')) {
            return false;
        }

        return std::all_of(Text.begin() + 1, Text.end(), [](char value) {
            const auto character = static_cast<unsigned char>(value);
            return std::isalnum(character) || character == '_';
        });
    }
};

enum class EHeaderVisibility {
    Public,
    Private
};

struct FPropertyDeclaration {
    std::string Name;
    std::string Flags;
};

struct FFunctionDeclaration {
    std::string Name;
    std::string Flags;
};

struct FClassDeclaration {
    std::string Name;
    std::string SuperName;
    std::size_t GeneratedBodyLine = 0;
    bool IsAbstract = false;
    std::vector<FPropertyDeclaration> Properties;
    std::vector<FFunctionDeclaration> Functions;
};

struct FHeaderUnit {
    fs::path InputPath;
    fs::path RelativePath;
    fs::path OutputPath;
    std::string IncludePath;
    std::string FileId;
    EHeaderVisibility Visibility = EHeaderVisibility::Public;
    std::vector<FClassDeclaration> Classes;
};

[[nodiscard]] std::string Trim(std::string value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }

    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

[[nodiscard]] std::string ReadTextFile(const fs::path& path) {
    std::ifstream stream(path, std::ios::binary);
    if (!stream) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }

    std::ostringstream builder;
    builder << stream.rdbuf();
    return builder.str();
}

void WriteTextFile(const fs::path& path, const std::string& content) {
    if (fs::exists(path)) {
        const std::string current = ReadTextFile(path);
        if (current == content) {
            return;
        }
    }

    fs::create_directories(path.parent_path());

    std::ofstream stream(path, std::ios::binary | std::ios::trunc);
    if (!stream) {
        throw std::runtime_error("Failed to write file: " + path.string());
    }

    stream << content;
}

[[nodiscard]] std::string SanitizeIdentifier(std::string value) {
    for (char& character : value) {
        const auto ascii = static_cast<unsigned char>(character);
        if (!std::isalnum(ascii)) {
            character = '_';
        }
    }

    if (value.empty() || std::isdigit(static_cast<unsigned char>(value.front()))) {
        value.insert(value.begin(), '_');
    }

    return value;
}

[[nodiscard]] std::string MakeFileId(std::string_view moduleName, const fs::path& relativePath) {
    std::string id = "FID_" + std::string(moduleName) + "_" + relativePath.generic_string();
    return SanitizeIdentifier(id);
}

[[nodiscard]] bool HasHeaderExtension(const fs::path& path) {
    const auto extension = path.extension().string();
    return extension == ".h" || extension == ".hpp";
}

[[nodiscard]] std::vector<fs::path> CollectHeaders(const fs::path& root) {
    std::vector<fs::path> headers;

    if (!fs::exists(root)) {
        return headers;
    }

    for (const auto& entry : fs::recursive_directory_iterator(root)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        if (HasHeaderExtension(entry.path())) {
            headers.push_back(fs::weakly_canonical(entry.path()));
        }
    }

    std::sort(headers.begin(), headers.end());
    return headers;
}

[[nodiscard]] std::vector<FToken> Tokenize(const std::string& source) {
    std::vector<FToken> tokens;
    std::size_t index = 0;
    std::size_t line = 1;

    auto pushToken = [&tokens](std::string text, std::size_t offset, std::size_t tokenLine) {
        tokens.push_back({std::move(text), offset, tokenLine});
    };

    while (index < source.size()) {
        const char current = source[index];

        if (current == '\r') {
            ++index;
            continue;
        }

        if (current == '\n') {
            ++line;
            ++index;
            continue;
        }

        if (std::isspace(static_cast<unsigned char>(current))) {
            ++index;
            continue;
        }

        if (current == '/' && index + 1 < source.size()) {
            const char next = source[index + 1];
            if (next == '/') {
                index += 2;
                while (index < source.size() && source[index] != '\n') {
                    ++index;
                }
                continue;
            }

            if (next == '*') {
                index += 2;
                while (index + 1 < source.size() && !(source[index] == '*' && source[index + 1] == '/')) {
                    if (source[index] == '\n') {
                        ++line;
                    }
                    ++index;
                }
                index = std::min(index + 2, source.size());
                continue;
            }
        }

        if (current == '"' || current == '\'') {
            const char quote = current;
            const std::size_t start = index;
            ++index;
            while (index < source.size()) {
                if (source[index] == '\\') {
                    index += 2;
                    continue;
                }

                if (source[index] == quote) {
                    ++index;
                    break;
                }

                if (source[index] == '\n') {
                    ++line;
                }
                ++index;
            }

            pushToken(source.substr(start, index - start), start, line);
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(current)) || current == '_') {
            const std::size_t start = index;
            ++index;
            while (index < source.size()) {
                const char value = source[index];
                const auto ascii = static_cast<unsigned char>(value);
                if (!std::isalnum(ascii) && value != '_') {
                    break;
                }
                ++index;
            }
            pushToken(source.substr(start, index - start), start, line);
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(current))) {
            const std::size_t start = index;
            ++index;
            while (index < source.size()) {
                const char value = source[index];
                const auto ascii = static_cast<unsigned char>(value);
                if (!std::isalnum(ascii) && value != '_') {
                    break;
                }
                ++index;
            }
            pushToken(source.substr(start, index - start), start, line);
            continue;
        }

        pushToken(std::string(1, current), index, line);
        ++index;
    }

    return tokens;
}

[[nodiscard]] std::size_t FindMatchingToken(const std::vector<FToken>& tokens, std::size_t openIndex, std::string_view openToken, std::string_view closeToken) {
    std::size_t depth = 0;

    for (std::size_t index = openIndex; index < tokens.size(); ++index) {
        if (tokens[index].Text == openToken) {
            ++depth;
        } else if (tokens[index].Text == closeToken) {
            --depth;
            if (depth == 0) {
                return index;
            }
        }
    }

    throw std::runtime_error("Unbalanced token sequence");
}

[[nodiscard]] std::string SliceText(const std::string& source, const std::vector<FToken>& tokens, std::size_t firstToken, std::size_t lastToken) {
    if (firstToken > lastToken || firstToken >= tokens.size() || lastToken >= tokens.size()) {
        return {};
    }

    const std::size_t start = tokens[firstToken].Offset;
    const std::size_t end = tokens[lastToken].Offset + tokens[lastToken].Text.size();
    return source.substr(start, end - start);
}

[[nodiscard]] std::optional<std::size_t> FindTopLevelToken(
    const std::vector<FToken>& tokens,
    std::size_t firstToken,
    std::size_t lastToken,
    std::string_view target) {
    std::size_t parenDepth = 0;
    std::size_t bracketDepth = 0;
    std::size_t braceDepth = 0;

    for (std::size_t index = firstToken; index <= lastToken && index < tokens.size(); ++index) {
        const std::string& text = tokens[index].Text;

        if (parenDepth == 0 && bracketDepth == 0 && braceDepth == 0 && text == target) {
            return index;
        }

        if (text == "(") {
            ++parenDepth;
        } else if (text == ")") {
            if (parenDepth > 0) {
                --parenDepth;
            }
        } else if (text == "[") {
            ++bracketDepth;
        } else if (text == "]") {
            if (bracketDepth > 0) {
                --bracketDepth;
            }
        } else if (text == "{") {
            ++braceDepth;
        } else if (text == "}") {
            if (braceDepth > 0) {
                --braceDepth;
            }
        }
    }

    return std::nullopt;
}

[[nodiscard]] std::string ParsePropertyName(const std::vector<FToken>& tokens, std::size_t firstToken, std::size_t lastToken) {
    std::size_t endToken = lastToken;

    if (const auto initializer = FindTopLevelToken(tokens, firstToken, lastToken, "=")) {
        endToken = *initializer;
    } else if (const auto braceInitializer = FindTopLevelToken(tokens, firstToken, lastToken, "{")) {
        endToken = *braceInitializer;
    }

    for (std::size_t index = endToken; index > firstToken; --index) {
        const std::size_t current = index - 1;
        if (tokens[current].IsIdentifier()) {
            return tokens[current].Text;
        }
    }

    throw std::runtime_error("Failed to parse property name");
}

[[nodiscard]] std::string ParseFunctionName(const std::vector<FToken>& tokens, std::size_t firstToken, std::size_t lastToken) {
    const auto openParen = FindTopLevelToken(tokens, firstToken, lastToken, "(");
    if (!openParen || *openParen == firstToken) {
        throw std::runtime_error("Failed to parse function declaration");
    }

    for (std::size_t index = *openParen; index > firstToken; --index) {
        const std::size_t current = index - 1;
        if (tokens[current].IsIdentifier()) {
            return tokens[current].Text;
        }
    }

    throw std::runtime_error("Failed to parse function name");
}

[[nodiscard]] bool ContainsAbstractSpecifier(const std::string& specifiers) {
    return specifiers.find("Abstract") != std::string::npos;
}

[[nodiscard]] FHeaderUnit ParseHeaderUnit(
    const FArguments& arguments,
    const fs::path& inputPath,
    EHeaderVisibility visibility,
    const fs::path& root,
    const fs::path& outputRoot) {
    const std::string source = ReadTextFile(inputPath);
    const std::vector<FToken> tokens = Tokenize(source);

    FHeaderUnit unit;
    unit.InputPath = inputPath;
    unit.RelativePath = fs::relative(inputPath, root);
    unit.OutputPath = outputRoot / (unit.RelativePath.stem().string() + ".generated.h");
    unit.IncludePath = unit.RelativePath.generic_string();
    unit.FileId = MakeFileId(arguments.ModuleName, (visibility == EHeaderVisibility::Public ? fs::path("Public") : fs::path("Private")) / unit.RelativePath);
    unit.Visibility = visibility;

    for (std::size_t index = 0; index < tokens.size(); ++index) {
        if (tokens[index].Text != "UCLASS") {
            continue;
        }

        if (index > 0 && tokens[index - 1].Text == "define") {
            continue;
        }

        if (index + 1 >= tokens.size() || tokens[index + 1].Text != "(") {
            throw std::runtime_error("Malformed UCLASS in " + inputPath.string());
        }

        const std::size_t specifierClose = FindMatchingToken(tokens, index + 1, "(", ")");
        const std::string specifiers = Trim(SliceText(source, tokens, index + 2, specifierClose > index + 2 ? specifierClose - 1 : index + 1));

        std::size_t cursor = specifierClose + 1;
        while (cursor < tokens.size() && tokens[cursor].Text != "class" && tokens[cursor].Text != "struct") {
            ++cursor;
        }

        if (cursor + 1 >= tokens.size()) {
            throw std::runtime_error("Expected class declaration after UCLASS in " + inputPath.string());
        }

        FClassDeclaration declaration;
        declaration.IsAbstract = ContainsAbstractSpecifier(specifiers);
        declaration.Name = tokens[cursor + 1].Text;

        std::size_t bodyOpen = cursor + 2;
        while (bodyOpen < tokens.size() && tokens[bodyOpen].Text != "{") {
            if (tokens[bodyOpen].Text == ":") {
                std::size_t inheritanceCursor = bodyOpen + 1;
                while (inheritanceCursor < tokens.size() && tokens[inheritanceCursor].Text != "{") {
                    if (tokens[inheritanceCursor].IsIdentifier() &&
                        tokens[inheritanceCursor].Text != "public" &&
                        tokens[inheritanceCursor].Text != "protected" &&
                        tokens[inheritanceCursor].Text != "private" &&
                        tokens[inheritanceCursor].Text != "final") {
                        declaration.SuperName = tokens[inheritanceCursor].Text;
                        break;
                    }
                    ++inheritanceCursor;
                }
            }
            ++bodyOpen;
        }

        if (bodyOpen >= tokens.size()) {
            throw std::runtime_error("Expected class body for " + declaration.Name);
        }

        const std::size_t bodyClose = FindMatchingToken(tokens, bodyOpen, "{", "}");
        std::size_t bodyCursor = bodyOpen + 1;
        std::size_t braceDepth = 1;

        while (bodyCursor < bodyClose) {
            const std::string& text = tokens[bodyCursor].Text;

            if (text == "{") {
                ++braceDepth;
                ++bodyCursor;
                continue;
            }

            if (text == "}") {
                --braceDepth;
                ++bodyCursor;
                continue;
            }

            if (braceDepth == 1 && text == "GENERATED_BODY") {
                if (bodyCursor + 1 >= tokens.size() || tokens[bodyCursor + 1].Text != "(") {
                    throw std::runtime_error("Malformed GENERATED_BODY in " + declaration.Name);
                }

                declaration.GeneratedBodyLine = tokens[bodyCursor].Line;
                bodyCursor = FindMatchingToken(tokens, bodyCursor + 1, "(", ")") + 1;
                continue;
            }

            if (braceDepth == 1 && text == "UPROPERTY") {
                if (bodyCursor + 1 >= tokens.size() || tokens[bodyCursor + 1].Text != "(") {
                    throw std::runtime_error("Malformed UPROPERTY in " + declaration.Name);
                }

                const std::size_t specifierEnd = FindMatchingToken(tokens, bodyCursor + 1, "(", ")");
                std::string flags = Trim(SliceText(source, tokens, bodyCursor + 2, specifierEnd > bodyCursor + 2 ? specifierEnd - 1 : bodyCursor + 1));
                if (flags.empty()) {
                    flags = "PF_None";
                }

                std::size_t declarationEnd = specifierEnd + 1;
                while (declarationEnd < bodyClose && tokens[declarationEnd].Text != ";") {
                    ++declarationEnd;
                }

                if (declarationEnd >= bodyClose) {
                    throw std::runtime_error("Unterminated property declaration in " + declaration.Name);
                }

                declaration.Properties.push_back({
                    ParsePropertyName(tokens, specifierEnd + 1, declarationEnd),
                    std::move(flags)
                });
                bodyCursor = declarationEnd + 1;
                continue;
            }

            if (braceDepth == 1 && text == "UFUNCTION") {
                if (bodyCursor + 1 >= tokens.size() || tokens[bodyCursor + 1].Text != "(") {
                    throw std::runtime_error("Malformed UFUNCTION in " + declaration.Name);
                }

                const std::size_t specifierEnd = FindMatchingToken(tokens, bodyCursor + 1, "(", ")");
                std::string flags = Trim(SliceText(source, tokens, bodyCursor + 2, specifierEnd > bodyCursor + 2 ? specifierEnd - 1 : bodyCursor + 1));
                if (flags.empty()) {
                    flags = "FUNC_None";
                }

                std::size_t declarationEnd = specifierEnd + 1;
                while (declarationEnd < bodyClose && tokens[declarationEnd].Text != ";") {
                    ++declarationEnd;
                }

                if (declarationEnd >= bodyClose) {
                    throw std::runtime_error("Unterminated function declaration in " + declaration.Name);
                }

                declaration.Functions.push_back({
                    ParseFunctionName(tokens, specifierEnd + 1, declarationEnd),
                    std::move(flags)
                });
                bodyCursor = declarationEnd + 1;
                continue;
            }

            ++bodyCursor;
        }

        if (declaration.GeneratedBodyLine == 0) {
            throw std::runtime_error("Missing GENERATED_BODY in " + declaration.Name);
        }

        unit.Classes.push_back(std::move(declaration));
        index = bodyClose;
    }

    return unit;
}

[[nodiscard]] std::vector<FHeaderUnit> ParseModuleHeaders(const FArguments& arguments) {
    std::vector<FHeaderUnit> headers;

    for (const auto& header : CollectHeaders(arguments.PublicRoot)) {
        headers.push_back(ParseHeaderUnit(arguments, header, EHeaderVisibility::Public, arguments.PublicRoot, arguments.OutputPublicRoot));
    }

    for (const auto& header : CollectHeaders(arguments.PrivateRoot)) {
        headers.push_back(ParseHeaderUnit(arguments, header, EHeaderVisibility::Private, arguments.PrivateRoot, arguments.OutputPrivateRoot));
    }

    std::sort(headers.begin(), headers.end(), [](const FHeaderUnit& lhs, const FHeaderUnit& rhs) {
        return lhs.InputPath < rhs.InputPath;
    });
    return headers;
}

[[nodiscard]] std::string BuildGeneratedBodyMacro(const FHeaderUnit& unit, const FClassDeclaration& declaration) {
    std::ostringstream output;
    output << "#define " << unit.FileId << "_" << declaration.GeneratedBodyLine << "_GENERATED_BODY \\\n";
    output << "public: \\\n";
    output << "    using ThisClass = " << declaration.Name << "; \\\n";
    if (!declaration.SuperName.empty()) {
        output << "    using Super = " << declaration.SuperName << "; \\\n";
    }
    output << "    static UClass* StaticClass(); \\\n";
    if (declaration.SuperName.empty()) {
        output << "    [[nodiscard]] virtual UClass* GetClass() const; \\\n";
    } else {
        output << "    [[nodiscard]] UClass* GetClass() const override; \\\n";
    }
    output << "private: \\\n";
    output << "    static void BuildReflectionData(UClass& outClass);";
    return output.str();
}

[[nodiscard]] std::string BuildGeneratedHeader(const FHeaderUnit& unit) {
    std::ostringstream output;
    output << "#pragma once\n\n";
    output << "#include \"VoxCore/Reflection/ObjectMacros.h\"\n\n";
    output << "#undef VOXCORE_CURRENT_FILE_ID\n";
    output << "#define VOXCORE_CURRENT_FILE_ID " << unit.FileId << "\n\n";

    for (std::size_t index = 0; index < unit.Classes.size(); ++index) {
        output << BuildGeneratedBodyMacro(unit, unit.Classes[index]) << "\n";
        if (index + 1 < unit.Classes.size()) {
            output << "\n";
        }
    }

    return output.str();
}

void AppendPropertyInitializers(std::ostringstream& output, const FClassDeclaration& declaration) {
    if (declaration.Properties.empty()) {
        output << "    outClass.Properties = {};\n";
        return;
    }

    output << "    outClass.Properties = {\n";
    for (std::size_t index = 0; index < declaration.Properties.size(); ++index) {
        const auto& property = declaration.Properties[index];
        output << "        VoxCore::Reflection::Core::MakePropertyInfo<&" << declaration.Name << "::" << property.Name << ">(\"" << property.Name << "\", " << property.Flags << ")";
        output << (index + 1 < declaration.Properties.size() ? ",\n" : "\n");
    }
    output << "    };\n";
}

void AppendFunctionInitializers(std::ostringstream& output, const FClassDeclaration& declaration) {
    if (declaration.Functions.empty()) {
        output << "    outClass.Functions = {};\n";
        return;
    }

    output << "    outClass.Functions = {\n";
    for (std::size_t index = 0; index < declaration.Functions.size(); ++index) {
        const auto& function = declaration.Functions[index];
        output << "        VoxCore::Reflection::Core::MakeFunctionInfo<&" << declaration.Name << "::" << function.Name << ">(\"" << function.Name << "\", " << function.Flags << ")";
        output << (index + 1 < declaration.Functions.size() ? ",\n" : "\n");
    }
    output << "    };\n";
}

[[nodiscard]] std::string BuildGeneratedSource(const FArguments& arguments, const std::vector<FHeaderUnit>& headers) {
    std::ostringstream output;
    output << "#include <type_traits>\n\n";
    output << "#include \"VoxCore/Reflection/IReflectionRegistry.h\"\n";
    output << "#include \"VoxCore/Reflection/ReflectionTypeUtils.h\"\n";

    for (const auto& header : headers) {
        if (!header.Classes.empty()) {
            output << "#include \"" << header.IncludePath << "\"\n";
        }
    }

    output << "\n";

    for (const auto& header : headers) {
        for (const auto& declaration : header.Classes) {
            output << "void " << declaration.Name << "::BuildReflectionData(UClass& outClass) {\n";
            output << "    outClass.ClassName = \"" << declaration.Name << "\";\n";
            if (declaration.SuperName.empty()) {
                output << "    outClass.SuperClass = nullptr;\n";
            } else {
                output << "    outClass.SuperClass = " << declaration.SuperName << "::StaticClass();\n";
            }
            AppendPropertyInitializers(output, declaration);
            AppendFunctionInitializers(output, declaration);
            output << "    if constexpr (std::is_default_constructible_v<" << declaration.Name << "> && !std::is_abstract_v<" << declaration.Name << ">) {\n";
            output << "        outClass.Construct = []() -> TUniquePtr<UObject> {\n";
            output << "            return MakeUnique<" << declaration.Name << ">();\n";
            output << "        };\n";
            output << "    } else {\n";
            output << "        outClass.Construct = {};\n";
            output << "    }\n";
            output << "}\n\n";

            output << "UClass* " << declaration.Name << "::StaticClass() {\n";
            output << "    static UClass cls = [] {\n";
            output << "        UClass value;\n";
            output << "        " << declaration.Name << "::BuildReflectionData(value);\n";
            output << "        return value;\n";
            output << "    }();\n";
            output << "    return &cls;\n";
            output << "}\n\n";

            output << "UClass* " << declaration.Name << "::GetClass() const {\n";
            output << "    return " << declaration.Name << "::StaticClass();\n";
            output << "}\n\n";
        }
    }

    output << "void " << arguments.RegistrationFunction << "(IReflectionRegistry& registry) {\n";
    for (const auto& header : headers) {
        for (const auto& declaration : header.Classes) {
            output << "    registry.RegisterClass(" << declaration.Name << "::StaticClass());\n";
        }
    }
    output << "}\n";

    return output.str();
}

[[nodiscard]] FArguments ParseArguments(int argc, char** argv) {
    FArguments arguments;

    for (int index = 1; index < argc; ++index) {
        const std::string_view key = argv[index];
        if (index + 1 >= argc) {
            throw std::runtime_error("Missing value for argument");
        }

        const std::string value = argv[++index];

        if (key == "--module-name") {
            arguments.ModuleName = value;
        } else if (key == "--module-root") {
            arguments.ModuleRoot = fs::weakly_canonical(value);
        } else if (key == "--public-root") {
            arguments.PublicRoot = fs::weakly_canonical(value);
        } else if (key == "--private-root") {
            arguments.PrivateRoot = fs::weakly_canonical(value);
        } else if (key == "--output-public-root") {
            arguments.OutputPublicRoot = fs::path(value);
        } else if (key == "--output-private-root") {
            arguments.OutputPrivateRoot = fs::path(value);
        } else if (key == "--registration-function") {
            arguments.RegistrationFunction = value;
        } else {
            throw std::runtime_error("Unknown argument: " + std::string(key));
        }
    }

    if (arguments.ModuleName.empty() ||
        arguments.ModuleRoot.empty() ||
        arguments.OutputPublicRoot.empty() ||
        arguments.OutputPrivateRoot.empty() ||
        arguments.RegistrationFunction.empty()) {
        throw std::runtime_error("Missing required arguments");
    }

    return arguments;
}

int main(int argc, char** argv) {
    try {
        const FArguments arguments = ParseArguments(argc, argv);
        const std::vector<FHeaderUnit> headers = ParseModuleHeaders(arguments);

        for (const auto& header : headers) {
            WriteTextFile(header.OutputPath, BuildGeneratedHeader(header));
        }

        const fs::path generatedSourcePath = arguments.OutputPrivateRoot / (arguments.ModuleName + ".Reflection.gen.cpp");
        WriteTextFile(generatedSourcePath, BuildGeneratedSource(arguments, headers));
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "VoxHeaderTool error: " << exception.what() << '\n';
        return 1;
    }
}
