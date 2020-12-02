#ifndef __header_h__
#define __header_h__

static const char * filehead = "#ifndef __attribute_h__\n\
#define __attribute_h__\n\
#include \"multisys.h\"\n\
namespace dc {\n\
    enum type {\n\
        type_s64,\n\
        type_string,\n\
\n\
        type_can_not_be_key,\n\
        type_float = type_can_not_be_key,\n\
        type_bool,\n\
    };\n\
\n\
    struct layout {\n\
        const std::string _name; \n\
        const s16 _index;\n\
        const s32 _offset;\n\
        const s8 _type;\n\
        const s16 _size;\n\
        const bool _visual; \n\
        const bool _share;\n\
        const bool _save;\n\
        const bool _important;\n\
\n\
        layout(const char * name, s16 index, s32 offset, s8 type, s16 size, bool visual, bool share, bool save, bool important) : _name(name), _index(index), _offset(offset), _type(type), _size(size), _visual(visual), _share(share), _save(save), _important(important) {}\n\
    };\n";

    static const char * fileend = "\n\
}\n\
#endif //__attribute_h__\n";

static const char * tshead = "export enum dc_value_type { \n\
    type_s64,\n\
    type_string,\n\
\n\
    type_can_not_be_key,\n\
    type_float = type_can_not_be_key,\n\
    type_bool,\n\
} \n\
 \n\
export class dc_layout { \n\
    readonly _name : string; \n\
    readonly _index : number; \n\
    readonly _type : dc_value_type;\n\
    readonly _visual : boolean;\n\
    readonly _share : boolean;\n\
    readonly _save : boolean;\n\
    readonly _important : boolean;\n\
\n\
    constructor(\n\
        name : string,\n\
        index : number,\n\
        type : dc_value_type,\n\
        visual : boolean,\n\
        share : boolean,\n\
        save : boolean,\n\
        important : boolean) {\n\
        this._name = name;\n\
        this._index = index;\n\
        this._type = type;\n\
        this._visual = visual;\n\
        this._share = share;\n\
        this._save = save;\n\
        this._important = important;\n\
    }\n\
}\n\
\n\
export var dc = {\n\
";

static const char* tsend = "}\n";
#endif //__header_h__
