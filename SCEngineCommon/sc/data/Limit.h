#ifndef _Limit_H_
#define _Limit_H_

#include <string>
#include <vector>
#include <algorithm>

namespace sc {

// The polymorphic base class of Limit
class LimitBase {
public:
    virtual ~LimitBase() { }
};

// The value limit rule for ScriptVar::value
template<typename T> class Limit : public LimitBase {
public:
    virtual T apply(const T& oldVal, const T& newVal) = 0;
};

// Limit int to some named values
class EnumLimit : public Limit<int> {
public:
    std::vector<std::string> mEnumNames;
    std::vector<int> mEnumValues;
    explicit EnumLimit(const std::vector<std::string>& enumNames) {
        std::vector<int> enumValues;
        for (int i = 0, size = static_cast<int>(enumNames.size()); i < size; i++) {
            enumValues.push_back(i);
        }
        new(this) EnumLimit(enumNames, enumValues);
    }
    EnumLimit(const std::vector<std::string>& enumNames, const std::vector<int>& enumValues) : mEnumNames(enumNames), mEnumValues(enumValues) { }
    int apply(const int& oldVal, const int& newVal) override {
        return std::find(mEnumValues.begin(), mEnumValues.end(), newVal) != mEnumValues.end() ? newVal : oldVal;
    }
};

// Limit int inside range
class IntRangeLimit : public Limit<int> {
public:
    const int mMin, mMax;
    IntRangeLimit(int min, int max) : mMin(min), mMax(max) { }
    int apply(const int& oldVal, const int& newVal) override {
        return std::min(std::max(newVal, mMin), mMax);
    }
};

// Limit float inside range
class FloatRangeLimit : public Limit<float> {
public:
    const float mMin, mMax;
    FloatRangeLimit(float min, float max) : mMin(min), mMax(max) { }
    float apply(const float& oldVal, const float& newVal) override {
        return std::min(std::max(newVal, mMin), mMax);
    }
};

} // namespace sc

#endif // _Limit_H_
