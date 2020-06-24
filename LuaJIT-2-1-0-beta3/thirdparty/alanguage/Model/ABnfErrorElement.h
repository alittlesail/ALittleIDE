
#ifndef _ALITTLE_ABNFERRORELEMENT_H_
#define _ALITTLE_ABNFERRORELEMENT_H_

#include <string>

#include "ABnfElement.h"

class ABnfErrorElement : public ABnfElement
{
private:
    std::string m_value = "";                   // 节点值
    ABnfElementPtr m_target;               // 本来要匹配的目标元素

public:
    ABnfErrorElement(ABnfFactory* factory, ABnfFile* file, int line, int col, int offset, const std::string& value, ABnfElementPtr target)
        : ABnfElement(factory, file, line, col, offset)
    {
        m_value = value;
        m_target = target;
    }

    bool IsLeafOrHasChildOrError() override {  return true; }
    bool IsError() override { return true; }

    ABnfElementPtr GetTargetElement() { return m_target; }

    // 获得节点错误内容
    inline const std::string& GetValue() { return m_value; }
    // 根据偏移位置，获取期望的元素
    ABnfElementPtr GetException(int offset) override
    {
        if (offset == GetStart()) return shared_from_this();
        return nullptr;
    }
}

#endif // _ALITTLE_ABNFERRORELEMENT_H_