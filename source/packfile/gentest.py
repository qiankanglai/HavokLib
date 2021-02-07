import yaml

g_gen_version = 1
g_fulldoc = {}


def load_schema(loaded_schema):
    if not loaded_schema.has_key('version'):
        raise RuntimeError('Missing version key!')

    schema_version = loaded_schema.pop('version')

    if g_gen_version < schema_version:
        raise RuntimeError('Version mismatch. Schema version: ' +
                           str(schema_version) + '. Expected <= ' + str(g_gen_version))
    g_fulldoc.update(loaded_schema)

    if g_fulldoc.has_key('import'):
        imports = g_fulldoc.pop('import')

        if(isinstance(imports, str)):
            load_schema(imports + '.yml')
        else:
            for i in imports:
                load_schema(i + '.yml')


def load_schema_from_file(file_name):
    loaded_schema = yaml.safe_load(file(file_name, 'r'))
    load_schema(loaded_schema)


def traceback_noarg(parent_key, func):
    try:
        func()
    except Exception as e:
        raise RuntimeError(str(e) + '\n\tTraceback: ' + str(parent_key))


def traceback(parent_key, func, args):
    try:
        func(args)
    except Exception as e:
        raise RuntimeError(str(e) + '\n\tTraceback: ' + str(parent_key))


def validate_template(tmpl_key):
    t_is_dict = isinstance(tmpl_key, dict)
    t_is_list = isinstance(tmpl_key, list)

    if not (t_is_dict or t_is_list):
        raise RuntimeError(
            'Invalid template ' + str(tmpl_key) + ' , expected array of key/vals or single key/val!')
    elif t_is_dict:
        for k, v in tmpl_key.items():
            if not isinstance(v, str):
                if len(v) != 1:
                    raise RuntimeError(
                        'Expected single val or key/val for ' + str(k))
                traceback(v, validate_template, v.values()[0])
    elif t_is_list:
        for l in tmpl_key:
            traceback(tmpl_key, validate_template, l)


def is_template_arg(tmpl_key, what):
    t_is_dict = isinstance(tmpl_key, dict)
    t_is_list = isinstance(tmpl_key, list)

    if t_is_dict:
        for l in tmpl_key.values():
            if l == what:
                return True
    elif t_is_list:
        for l in tmpl_key:
            if is_template_arg(l, what):
                return True
    return False


def validate_template_arg(tmpl_key, what):
    if not is_template_arg(tmpl_key, what):
        raise RuntimeError(
            str(what) + ' is not a name of template argumemnt!')

def validate_required_keys(holder, item, what):
    for k in what:
        if not item.has_key(k):
            raise RuntimeError(k + ' key is required for primitive: ' + holder)

def validate_primitive(what):
    k, v = what
    validate_required_keys(k, v, ['size', 'alignment'])
    k_size = v['size']
    k_align = v['alignment']
    is_size_num = isinstance(k_size, int)
    is_align_num = isinstance(k_align, int)
    k_templ = v['template'] if v.has_key('template') else None
    if k_templ:
        traceback(v, validate_template, k_templ)
    if is_size_num and is_align_num:
        if k_size < k_align:
            raise RuntimeError(
                'alignment must be <= size for primitive: ' + k)
    else:
        if not k_templ:
            raise RuntimeError('Expected template key!')
        if not is_size_num:
            validate_template_arg(k_templ, k_size)
        if not is_align_num:
            validate_template_arg(k_templ, k_align)


def validate_primitives():
    for k, v in g_fulldoc.items():
        if v.has_key('type') and v['type'] == 'primitive':
            traceback((k, v), validate_primitive, (k, v))


# load_schema('basetypes.yml')
# load_schema('hkx.yml') test_primitive

loaded_schema = yaml.safe_load_all(file('test_primitive.yml', 'r'))

for s in loaded_schema:
    g_fulldoc.clear()
    try:
        load_schema(s)
        validate_primitives()
    except Exception as e:
        print(e)
        pass
    else:
        raise RuntimeError('Expected raise!\n' + str(s))
