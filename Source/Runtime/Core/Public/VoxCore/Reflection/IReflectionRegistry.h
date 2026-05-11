#pragma once

class UClass;

class IReflectionRegistry {
public:
    virtual ~IReflectionRegistry() = default;

    virtual void RegisterClass(UClass* cls) = 0;
};
