
#include "ALittleScriptGuessClass.h"

#include "../Index/ALittleScriptUtility.h"

#include "../Generate/ALittleScriptClassDecElement.h"

ALittleScriptGuessClass::ALittleScriptGuessClass(const std::string& p_namespace_name, const std::string& p_class_name, std::shared_ptr<ALittleScriptClassDecElement> p_class_dec, const std::string& p_using_name, bool p_is_const, bool p_is_native)
{
    is_register = ALittleScriptUtility::IsRegister(p_class_dec);
    namespace_name = p_namespace_name;
    class_name = p_class_name;
    class_dec = p_class_dec;
    using_name = p_using_name;
    is_const = p_is_const;
    is_native = p_is_native;
}

std::shared_ptr<ABnfElement> ALittleScriptGuessClass::GetElement() { return class_dec.lock(); }

bool ALittleScriptGuessClass::NeedReplace() const
{
    if (template_list.empty()) return false;

    for (auto& pair : template_map)
    {
        auto guess = pair.second.lock();
        if (guess == nullptr) return false;

        if (guess->NeedReplace()) return true;
    }
    return false;
}

ABnfGuessPtr ALittleScriptGuessClass::ReplaceTemplate(const std::unordered_map<std::string, ABnfGuessPtr>& fill_map) const
{
    auto new_guess = std::dynamic_pointer_cast<ALittleScriptGuessClass>(Clone());
    for (auto& pair : template_map)
    {
        auto template_guess = pair.second.lock();
        if (template_guess == nullptr) return nullptr;

        auto guess = template_guess->ReplaceTemplate(fill_map);
        if (guess == nullptr) return nullptr;
        if (guess != template_guess)
        {
            auto replace = template_guess->ReplaceTemplate(fill_map);
            if (replace == nullptr) return nullptr;
            new_guess->template_map[pair.first] = replace;
        }
    }
    return new_guess;
}

ABnfGuessPtr ALittleScriptGuessClass::Clone() const
{
    auto guess = std::shared_ptr< ALittleScriptGuessClass>(new ALittleScriptGuessClass(namespace_name, class_name, class_dec.lock(), using_name, is_const, is_native));
    guess->template_list = template_list;
    guess->template_map = template_map;
    guess->UpdateValue();
    return guess;
}

void ALittleScriptGuessClass::UpdateValue()
{
    value = "";
    if (is_native) value += "native ";
    value += namespace_name + "." + class_name;
    std::vector<std::string> name_list;
    for (auto template_guess : template_list)
    {
        if (template_map.TryGetValue(template.GetValueWithoutConst(), out ABnfGuess impl))
        {
            if (template.is_const && !impl.is_const)
            {
                impl = impl.Clone();
                impl.is_const = true;
                impl.UpdateValue();
            }
            name_list.Add(impl.GetValue());
        }
        else
            name_list.Add(template.GetValue());
    }
    if (name_list.Count > 0)
        value += "<" + string.Join(",", name_list) + ">";

    value_without_const = value;
    if (is_const) value = "const " + value;
}

bool ALittleScriptGuessClass::IsChanged() const
{
    for (auto& template_guess : template_list)
    {
        auto guess = template_guess.lock();
        if (guess == nullptr || guess->IsChanged())
            return true;
    }
    for (auto& pair : template_map)
    {
        auto guess = pair.second.lock();
        if (guess == nullptr || guess->IsChanged())
            return true;
    }

    auto class_dec_element = class_dec.lock();
    if (class_dec_element == nullptr) return true;

    return dynamic_cast<ALittleScriptIndex*>(class_dec_element->GetFile()->GetProject())->GetGuessTypeList(class_dec) == nullptr;
}
