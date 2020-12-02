#include "tools.h"
#include <string>
#include <unordered_map>
#include "header.h"
#include "cfile.h"
#include <map>
#include <unordered_set>
#include "tinyxml/tinyxml2.h"

using namespace std;

typedef unordered_map<string, string> input_args_map;

static input_args_map _input_args;
static std::string _out_path;

void parse(int argc, const char ** argv) {
    for (int i = 1; i < argc; ++i) {
        tassert(strncmp(argv[i], "--", 2) == 0, "invalid argv %s", argv[i]);

        const char * start = argv[i] + 2;
        const char * equal = strstr(start, "=");
        tassert(equal != nullptr, "invalid argv %s", argv[i]);
        std::string name(start, equal);
        std::string val(equal + 1);
        _input_args[name] = val;
    }
}

const char * getInputArg(const char * name) {
    input_args_map::const_iterator itor = _input_args.find(name);
    if (itor == _input_args.end()) {
        return nullptr;
    }

    return itor->second.c_str();
}

static std::map<string, string> s_path_map;
static std::map<string, string> s_attr_map;
static std::map<string, s32> s_index_map;
static std::map<string, s32> s_offset_map;
static std::map<string, string> s_table_map;
static std::map<string, s32> s_table_tag;


static std::map<string, string> s_ts_attr_map;
static std::map<string, s32> s_ts_index_map;
static std::map<string, s32> s_ts_offset_map;
static std::map<string, string> s_ts_table_map;
static std::map<string, s32> s_ts_table_tag;

string convertProp(const char * name, s32 & index, s32 & offset) {
    std::map<string, string>::iterator itor = s_attr_map.find(name);
    if (itor != s_attr_map.end()) {
        offset = s_offset_map.find(name)->second;
        index = s_index_map.find(name)->second;
        return itor->second;
    }

    std::map<string, string>::iterator ifind = s_path_map.find(name);
    if (ifind == s_path_map.end()) {
        tassert(false, "can not find %s.xml", name);
        return "";
    }

    string value = "";
    tinyxml2::XMLDocument xml;
    if (tinyxml2::XML_SUCCESS == xml.LoadFile(ifind->second.c_str())) {
        tinyxml2::XMLElement * root = xml.RootElement();
        const char * parent = root->Attribute("parent");
        if (parent) {
            value = convertProp(parent, index, offset);
        }

        tinyxml2::XMLElement * prop = root->FirstChildElement("attribute");

        unordered_set<string> prop_name_set;
        while (prop) {
            const char * name = prop->Attribute("name");
            if (prop_name_set.find(name) != prop_name_set.end()) {
                tassert(false, "wtf");
                exit(-1);
            }
            prop_name_set.insert(name);

            const char * type = prop->Attribute("type");
            const char * visual = prop->Attribute("visual");
            const char * share = prop->Attribute("share");
            const char * save = prop->Attribute("save");
            const char * important = prop->Attribute("important");
            tassert(name && type && share && important, "wtf");

            s32 size = 0;
            if (!strcmp(type, "s64")) {
                size = sizeof(s64);
            } else if (!strcmp(type, "float")) {
                size = sizeof(float);
            } else if (!strcmp(type, "string")) {
                size = tools::stringAsInt(prop->Attribute("size"));
            } else if (!strcmp(type, "bool")) {
                size = sizeof(bool);
            } else {
                tassert(false, "wtf");
                continue;
            }

            //static const attribute UserID(0, type_s64, sizeof(s64), false, false);
            std::string temp;
            temp.append("               static const layout ").append(name).append("(\"")
                .append(name).append("\", ").append(tools::intAsString(index)).append(", ")
                .append(tools::intAsString(offset)).append(", type_").append(type).append(", ")
                .append(tools::intAsString(size)).append(", ").append(visual).append(", ").append(share)
                .append(", ").append(save).append(", ").append(important).append(");\n");

            offset += size;
            value += temp.c_str();
            index++;
            prop = prop->NextSiblingElement("attribute");
        }
    }

    s_offset_map.insert(make_pair(name, offset));
    s_attr_map.insert(make_pair(name, value));
    s_index_map.insert(make_pair(name, index));
    return value;
}

string convertPropTs(const char* name, s32& index, s32& offset) {
    std::map<string, string>::iterator itor = s_ts_attr_map.find(name);
    if (itor != s_ts_attr_map.end()) {
        offset = s_ts_offset_map.find(name)->second;
        index = s_ts_index_map.find(name)->second;
        return itor->second;
    }

    std::map<string, string>::iterator ifind = s_path_map.find(name);
    if (ifind == s_path_map.end()) {
        tassert(false, "can not find %s.xml", name);
        return "";
    }

    string value = "";
    tinyxml2::XMLDocument xml;
    if (tinyxml2::XML_SUCCESS == xml.LoadFile(ifind->second.c_str())) {
        tinyxml2::XMLElement* root = xml.RootElement();
        const char* parent = root->Attribute("parent");
        if (parent) {
            value = convertPropTs(parent, index, offset);
        }

        tinyxml2::XMLElement* prop = root->FirstChildElement("attribute");

        unordered_set<string> prop_name_set;
        while (prop) {
            const char* name = prop->Attribute("name");
            if (prop_name_set.find(name) != prop_name_set.end()) {
                tassert(false, "wtf");
                exit(-1);
            }
            prop_name_set.insert(name);

            const char* type = prop->Attribute("type");
            const char* visual = prop->Attribute("visual");
            const char* share = prop->Attribute("share");
            const char* save = prop->Attribute("save");
            const char* important = prop->Attribute("important");
            tassert(name && type && share && important, "wtf");

            //static const attribute UserID(0, type_s64, sizeof(s64), false, false);
            std::string temp;
            temp.append("                ").append(name).append(": new dc_layout(\"")
                .append(name).append("\", ").append(tools::intAsString(index)).append(", ").append("dc_value_type.type_").append(type)
                .append(", ").append(visual).append(", ").append(share)
                .append(", ").append(save).append(", ").append(important).append("),\n");

            value += temp.c_str();
            index++;
            prop = prop->NextSiblingElement("attribute");
        }
    }

    s_ts_offset_map.insert(make_pair(name, offset));
    s_ts_attr_map.insert(make_pair(name, value));
    s_ts_index_map.insert(make_pair(name, index));
    return value;
}

string ConvertTable(const char* name, s32& tag) {
    std::map<string, string>::iterator itor = s_table_map.find(name);
    if (itor != s_table_map.end()) {
        std::map<string, s32>::iterator itag = s_table_tag.find(name);
        tag = itag->second;
        return itor->second;
    }

    std::map<string, string>::iterator ifind = s_path_map.find(name);
    if (ifind == s_path_map.end()) {
        tassert(false, "can not find %s.xml", name);
        return "";
    }

    string value = "";
    tinyxml2::XMLDocument xml;
    if (tinyxml2::XML_SUCCESS == xml.LoadFile(ifind->second.c_str())) {
        tinyxml2::XMLElement* root = xml.RootElement();
        const char* parent = root->Attribute("parent");
        if (parent) {
            value = ConvertTable(parent, tag);
        }

        tinyxml2::XMLElement* tab = root->FirstChildElement("table");
        while (tab) {
            const char* name = tab->Attribute("name");
            value += "            namespace " + string(name) + " {\n";
            value += "                static const char * name = \"" + string(name) + "\";\n";
            value += "                static const s32 tag = " + string(tools::intAsString(tag++).c_str()) + ";\n";
            value += "                enum {\n";
            tinyxml2::XMLElement* column = tab->FirstChildElement("column");
            bool haskey = false;
            unordered_set<string> column_name_set;
            while (column) {
                const char* column_name = column->Attribute("name");
                if (column_name_set.find(column_name) != column_name_set.end()) {
                    tassert(false, "wtf");
                    exit(-1);
                }

                column_name_set.insert(column_name);

                const char* type = column->Attribute("type");
                bool key = false;
                if (column->Attribute("key")) {
                    tassert(!haskey, "wtf");
                    haskey = true;
                    key = true;
                }

                std::string temp;
                temp.append("                    column_").append(column_name).append("_");
                if (!strcmp(type, "s64")) {
                    temp.append("int64");
                }
                else if (!strcmp(type, "string")) {
                    const char* size = column->Attribute("size");
                    temp.append("string_size").append(size);
                }
                else if (!strcmp(type, "float")) {
                    temp.append("float");
                    tassert(!key, "wtf");
                }
                else if (!strcmp(type, "bool")) {
                    temp.append("bool");
                    tassert(!key, "wtf");
                }
                else {
                    tassert(false, "wtf");
                    continue;
                }

                if (key) {
                    temp.append("_key");
                }
                temp.append(",\n");

                value += temp.c_str();
                column = column->NextSiblingElement("column");
            }
            value += "                };\n";
            value += "            }\n";
            tab = tab->NextSiblingElement("table");
        }
    }

    s_table_map.insert(make_pair(name, value));
    s_table_tag.insert(make_pair(name, tag));
    return value;
}

string ConvertTableTs(const char* name, s32& tag) {
    std::map<string, string>::iterator itor = s_ts_table_map.find(name);
    if (itor != s_ts_table_map.end()) {
        std::map<string, s32>::iterator itag = s_ts_table_tag.find(name);
        tag = itag->second;
        return itor->second;
    }

    std::map<string, string>::iterator ifind = s_path_map.find(name);
    if (ifind == s_path_map.end()) {
        tassert(false, "can not find %s.xml", name);
        return "";
    }

    string value = "";
    tinyxml2::XMLDocument xml;
    if (tinyxml2::XML_SUCCESS == xml.LoadFile(ifind->second.c_str())) {
        tinyxml2::XMLElement* root = xml.RootElement();
        const char* parent = root->Attribute("parent");
        if (parent) {
            value = ConvertTableTs(parent, tag);
        }

        tinyxml2::XMLElement* tab = root->FirstChildElement("table");
        while (tab) {
            const char* name = tab->Attribute("name");
            value += "                " + string(name) + " : {\n";
            value += "                    name : \"" + string(name) + "\",\n";
            value += "                    tag : " + string(tools::intAsString(tag++).c_str()) + ",\n";
            value += "                    column : {\n";
            tinyxml2::XMLElement* column = tab->FirstChildElement("column");
            bool haskey = false;
            unordered_set<string> column_name_set;
            int ts_column_index = 0;
            while (column) {
                const char* column_name = column->Attribute("name");
                if (column_name_set.find(column_name) != column_name_set.end()) {
                    tassert(false, "wtf");
                    exit(-1);
                }

                column_name_set.insert(column_name);

                const char* type = column->Attribute("type");
                bool key = false;
                if (column->Attribute("key")) {
                    tassert(!haskey, "wtf");
                    haskey = true;
                    key = true;
                }

                std::string temp;
                temp.append("                        ").append(column_name).append("_");
                if (!strcmp(type, "s64")) {
                    temp.append("int64");
                }
                else if (!strcmp(type, "string")) {
                    const char* size = column->Attribute("size");
                    temp.append("string_size").append(size);
                }
                else if (!strcmp(type, "float")) {
                    temp.append("float");
                    tassert(!key, "wtf");
                }
                else if (!strcmp(type, "bool")) {
                    temp.append("bool");
                    tassert(!key, "wtf");
                }
                else {
                    tassert(false, "wtf");
                    continue;
                }

                if (key) {
                    temp.append("_key");
                }
                temp.append(" : { index :").append(tools::intAsString(ts_column_index).c_str()).append(", type : dc_value_type.type_").append(type).append(", key : ").append(tools::boolAsString(key)).append("},\n");

                value += temp.c_str();
                column = column->NextSiblingElement("column");

                ts_column_index++;
            }
            value += "                    }\n";
            value += "                },\n";
            tab = tab->NextSiblingElement("table");
        }
    }

    s_ts_table_map.insert(make_pair(name, value));
    s_ts_table_tag.insert(make_pair(name, tag));
    return value;
}

string ConvertStaticTable(const char * path) {
    tinyxml2::XMLDocument xml;
    if (tinyxml2::XML_SUCCESS != xml.LoadFile(path)) {
        tassert(false, "wtf");
        return "";
    }

    tinyxml2::XMLElement * root = xml.RootElement();
    tinyxml2::XMLElement * tab = root->FirstChildElement("table");
    string value = "";
    s32 tag = 0;
    while (tab) {
        const char * name = tab->Attribute("name");
        value += "        namespace " + string(name) + "{\n";
        value += "            static const char * name = \"" + string(name) + "\";\n";
        value += "            static const s32 tag = " + string(tools::intAsString(tag++).c_str()) + ";\n";
        value += "            enum {\n";
        tinyxml2::XMLElement * column = tab->FirstChildElement("column");
        bool haskey = false;
        unordered_set<string> column_name_set;
        while (column) {
            const char * column_name = column->Attribute("name");
            if (column_name_set.find(column_name) != column_name_set.end()) {
                tassert(false, "wtf");
                exit(-1);
            }

            column_name_set.insert(column_name);

            const char * type = column->Attribute("type");
            bool key = false;
            if (column->Attribute("key")) {
                tassert(!haskey, "wtf");
                haskey = true;
                key = true;
            }

            std::string temp;
            temp.append("                column_").append(column_name).append("_");
            if (!strcmp(type, "s64")) {
                temp.append("int64");
            }
            else if (!strcmp(type, "string")) {
                const char * size = column->Attribute("size");
                temp.append("string_size").append(size);
            }
            else if (!strcmp(type, "float")) {
                temp.append("float");
                tassert(!key, "wtf");
            }
            else if (!strcmp(type, "bool")) {
                temp.append("bool");
                tassert(!key, "wtf");
            }
            else {
                tassert(false, "wtf");
                continue;
            }

            if (key) {
                temp.append("_key");
            }
            temp.append(",\n");

            value += temp.c_str();
            column = column->NextSiblingElement("column");
        }
        value += "            };\n";
        value += "        }\n";
        tab = tab->NextSiblingElement("table");
    }

    return value;
}

int main(int argc, const char ** args, const char ** env) {
    parse(argc, args);

    if (getInputArg("pause")) {
        getchar();
    }

    const char * attr_path = getInputArg("attr_path");
    const char * out_path = getInputArg("out_path");

    std::string inpath = tools::file::getApppath();
    inpath.append(attr_path);

    std::string outpath = tools::file::getApppath();
    outpath.append("/").append(out_path).append("attribute.h");

    std::string ts_outpath = tools::file::getApppath();
    ts_outpath.append("/").append(out_path).append("attribute.ts");

    printf("%s\n", outpath.c_str());
    printf("%s\n", ts_outpath.c_str());

    tlib::cfile file(outpath.c_str(), true);
    file << filehead;

    tlib::cfile ts_file(ts_outpath.c_str(), true);
    ts_file << tshead;

    tools::file::opaths paths;
    tools::file::onames names;
    s32 count = 0;

    std::string objectpath; 
    objectpath.append(inpath.c_str()).append("/object");
    std::string statictablepath;
    statictablepath.append(inpath.c_str()).append("/global.xml");

    if (tools::file::getfiles(objectpath.c_str(), ".xml", paths, names, count)) {
        file << "\n    static const char * files[] = {";
        ts_file << "\n      files : [";
        for (s32 i = 0; i < count; i++) {
            file << "\"" << names[i].c_str() << "\"";
            ts_file << "\"" << names[i].c_str() << "\"";
            if (i != count - 1) {
                file << ",";
                ts_file << ",";
            }
        }
        file << "};\n";
        ts_file << "],\n";

        for (s32 i = 0; i < count; i++) {
            s_path_map.insert(make_pair(names[i], paths[i]));
        }

        ts_file << "        o  : {\n";

        for (s32 i = 0; i < count; i++) {
            file << "    namespace " << names[i].c_str() << " {\n";
            ts_file << "        " << names[i].c_str() << " : {\n";

            file << "        static const char * name = \"" << names[i].c_str() << "\";\n";
            ts_file << "            name : \"" << names[i].c_str() << "\",\n";

            file << "        namespace attr {\n";
            ts_file << "            attr : {\n";

            s32 index = 0;
            s32 offset = 0;
            string value = convertProp(names[i].c_str(), index, offset);
            file << value.c_str();
            file << "        }\n\n";

            s32 ts_index = 0;
            s32 ts_offset = 0;
            string ts_value = convertPropTs(names[i].c_str(), ts_index, ts_offset);
            ts_file << ts_value.c_str();
            ts_file << "            },\n";

            file << "        namespace table {\n";
            ts_file << "            table : {\n";

            s32 tag = 0;
            value = ConvertTable(names[i].c_str(), tag);
            file << value.c_str();
            file << "        }\n";
            file << "    }\n";

            s32 ts_tag = 0;
            ts_value = ConvertTableTs(names[i].c_str(), ts_tag);
            ts_file << ts_value.c_str();
            ts_file << "            }\n";
            ts_file << "        },\n\n";
        }
        ts_file << "    }\n";

        file << "\n    namespace global {\n";
        file << ConvertStaticTable(statictablepath.c_str()).c_str();
        file << "    }";
    }

    file << fileend;
    file.save();
    file.close();

    ts_file << tsend;
    ts_file.save();
    file.close();

    return 0;
}
