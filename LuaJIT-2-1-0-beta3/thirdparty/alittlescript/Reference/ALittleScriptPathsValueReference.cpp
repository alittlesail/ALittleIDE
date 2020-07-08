#include "ALittleScriptPathsValueReference.h"

#include "../Generate/ALittleScriptTextElement.h"

#include "../Guess/ALittleScriptGuessList.h"
#include "../Index/ALittleScriptIndex.h"
#include "../Index/ALittleScriptUtility.h"
#include "../Index/ALittleScriptFileClass.h"

ABnfGuessError ALittleScriptPathsValueReference::GuessTypes(std::vector<ABnfGuessPtr>& guess_list)
{
    auto element = m_element.lock();
    if (element == nullptr) return ABnfGuessError(nullptr, u8"�ڵ�ʧЧ");
    auto info = ABnfGuessPtr(new ALittleScriptGuessList(ALittleScriptStatic::Inst().sStringGuess, false, false));
    info->UpdateValue();
    element->GetFile()->AddGuessType(info);
    guess_list.push_back(info);
    return nullptr;
}

ABnfGuessError ALittleScriptPathsValueReference::CheckError()
{
    auto element = m_element.lock();
    if (element == nullptr) return ABnfGuessError(nullptr, u8"�ڵ�ʧЧ");
    auto text = element->GetText();
    if (text == nullptr) return ABnfGuessError(element, u8"����д·������ȡ���ļ����Լ��ļ���·��");

    // ���·���Ƿ����
    auto path = element->GetProjectPath() + text->GetElementString();
    ALittleScriptUtility::TrimLeft(path);
    ALittleScriptUtility::TrimRight(path);
    if (!ALittleScriptUtility::IsDirExist(path))
        return ABnfGuessError(element, u8"��д��·��������:" + path);
    return nullptr;
}