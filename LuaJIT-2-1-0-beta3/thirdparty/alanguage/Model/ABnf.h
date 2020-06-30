
#ifndef _ALITTLE_ABNF_H_
#define _ALITTLE_ABNF_H_

#include <vector>
#include <unordered_map>
#include <string>
#include <regex>
#include "ABnfRule.h"
#include "ALanguageHelperInfo.h"

class ABnfNodeElement;
class ABnfFile;

class ABnfFactory;
using ABnfNodeElementPtr = std::shared_ptr<ABnfNodeElement>;
class ABnfErrorElement;
using ABnfErrorElementPtr = std::shared_ptr<ABnfErrorElement>;
class ABnfKeyElement;
using ABnfKeyElementPtr = std::shared_ptr<ABnfKeyElement>;
class ABnfStringElement;
using ABnfStringElementPtr = std::shared_ptr<ABnfStringElement>;
class ABnfRegexElement;
using ABnfRegexElementPtr = std::shared_ptr<ABnfRegexElement>;

class ABnf
{
private:
    // ABnf�������
    ABnfRule m_rule;
    ABnfRuleInfo* m_root = nullptr;            // �������
    ABnfRuleInfo* m_line_comment = nullptr;    // ����ע��
    ABnfRuleInfo* m_block_comment = nullptr;   // ����ע��

    // �ڵ㹤��
    ABnfFactory* m_factory = nullptr;

    // ���ڽ����Ĵ���
    ABnfFile* m_file = nullptr;
    // �Ѿ���֤�����������ڻ���
    std::unordered_map<int, std::unordered_map<ABnfRuleNodeInfo*, int>> m_regex_skip;
    // �Ѿ���֤���ĵ���ע�ͣ����ڻ���
    std::unordered_set<int> m_line_comment_skip;
    // �Ѿ���֤���Ķ���ע�ͣ����ڻ���
    std::unordered_set<int> m_block_comment_skip;
    // ������ջ
    std::vector<ABnfRuleInfo*> m_stop_stack;
    
    // ���ż��ϣ���ǰ����������������ַ�����ô��ƥ��ʧ��
    std::unordered_map<std::string, std::unordered_set<char>> m_symbol_check;

public:
    // ��ʼ������
    void Clear();

    // �����ķ�
    bool Load(const std::string& buffer, ABnfFactory* factory, std::string& error);

    // ���ҹ���
    ABnfRuleInfo* GetRule(const std::string& name);

    // ��ѯ�ؼ���
    void QueryKeyWordCompletion(const std::string& input, std::vector<ALanguageCompletionInfo>& list);
    // �����ڵ�
    ABnfNodeElementPtr CreateNodeElement(int line, int col, int offset, const std::string& type);
    ABnfKeyElementPtr CreateKeyElement(int line, int col, int offset, const std::string& value);
    ABnfStringElementPtr CreateStringElement(int line, int col, int offset, const std::string& value);
    ABnfRegexElementPtr CreateRegexElement(int line, int col, int offset, const std::string& value, std::shared_ptr<std::regex> regex);

    // �����ļ�
    ABnfNodeElementPtr Analysis(ABnfFile* file);

    // �����������
    bool AnalysisABnfNode(ABnfRuleInfo* rule, ABnfRuleNodeInfo* node, ABnfNodeElementPtr parent, bool not_key
        , int& line, int& col, int& offset, int& pin_offset, bool ignore_error);

    bool AnalysisABnfNodeMore(ABnfRuleInfo* rule
        , ABnfRuleNodeInfo* node, ABnfNodeElementPtr parent, bool not_key
        , int& line, int& col, int& offset, int& pin_offset, bool ignore_error);

    // ����ڵ�
    bool AnalysisABnfRuleMatch(ABnfRuleInfo* rule, ABnfNodeElementPtr parent, bool not_key
        , int& line, int& col, int& offset, int& pin_offset, bool ignore_error);

    // �����ڵ�
    bool AnalysisABnfNodeMatch(ABnfRuleInfo* rule
        , ABnfRuleNodeInfo* node, ABnfNodeElementPtr parent, bool not_key
        , int& line, int& col, int& offset, int& pin_offset, bool ignore_error);

    // �ؼ���ƥ��
    bool AnalysisABnfKeyMatch(ABnfRuleInfo* rule
        , ABnfRuleNodeInfo* node, ABnfNodeElementPtr parent, bool not_key
        , int& line, int& col, int& offset, bool ignore_error);

    // �ַ���ƥ��
    bool AnalysisABnfStringMatch(ABnfRuleInfo* rule
        , ABnfRuleNodeInfo* node, ABnfNodeElementPtr parent, bool not_key
        , int& line, int& col, int& offset, bool ignore_error);

    // �������ʽƥ��
    bool AnalysisABnfRegexMatch(ABnfRuleInfo* rule
        , ABnfRuleNodeInfo* node, ABnfNodeElementPtr parent, bool not_key
        , int& line, int& col, int& offset, int& pin_offset, bool ignore_error);

    // ��ע��ƥ��
    bool AnalysisABnfCommentMatch(ABnfRuleInfo* rule, ABnfNodeElementPtr parent, bool not_key, int& line, int& col, int& offset);

    // ���ݽ��յĴ�С������ƫ��
    void AnalysisOffset(int value_len, int& line, int& col, int& offset);

    // ������һ��
    bool JumpToNextLine(int& line, int& col, int& offset);

    // ���и��ַ�������Ծ
    void AnalysisSkip(int& line, int& col, int& offset);
};

#endif // _ALITTLE_ABNF_H_