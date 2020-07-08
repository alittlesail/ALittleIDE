
#include "ALittleScriptFileClass.h"
#include "ALittleScriptProjectClass.h"
#include "ALittleScriptIndex.h"

#include "../../alanguage/Model/ABnf.h"
#include "../../alanguage/Model/ABnfNodeElement.h"

ALittleScriptFileClass::ALittleScriptFileClass(ABnfProject* project, const std::string& full_path, const char* text, size_t len, bool in_ui)
	: ABnfFile(project, full_path, text, len, in_ui)
{
}

ALittleScriptFileClass::~ALittleScriptFileClass()
{
}

// ���·�������
void ALittleScriptFileClass::UpdateAnalysis()
{
    m_root = nullptr;
    if (m_in_ui)
        m_root = GetProject()->RefABnfUI().Analysis(this);
    else
        m_root = GetProject()->RefABnf().Analysis(this);
}

void ALittleScriptFileClass::UpdateError()
{
    ClearCheckError();
    ClearAnalysisError();

    if (m_root == nullptr) return;

    CollectError(m_root);
    AnalysisError(m_root);
}

void ALittleScriptFileClass::OnBeforeUpdate()
{
    auto* index = dynamic_cast<ALittleScriptIndex*>(m_project);
    if (index == nullptr) return;
    if (m_root == nullptr) return;
    index->RemoveRoot(std::dynamic_pointer_cast<ALittleScriptRootElement>(m_root));
}

void ALittleScriptFileClass::OnAfterUpdate()
{
    auto* index = dynamic_cast<ALittleScriptIndex*>(m_project);
    if (index == nullptr) return;
    UpdateAnalysis();
    if (m_root == nullptr) return;
    index->AddRoot(std::dynamic_pointer_cast<ALittleScriptRootElement>(m_root));
}

void ALittleScriptFileClass::OnRemove()
{
    auto* index = dynamic_cast<ALittleScriptIndex*>(m_project);
    if (index == nullptr) return;
    if (m_root == nullptr) return;
    index->RemoveRoot(std::dynamic_pointer_cast<ALittleScriptRootElement>(m_root));
}

