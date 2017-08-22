import coreir

def test_load_core(libs, files):
    context = coreir.Context()
    for lib in libs:
        context.load_library(lib)

    for file in files:
        top_module = context.load_from_file(file)
        assert top_module is not None
        top_def = top_module.definition
        assert top_def is not None
        modules = dict()

        for inst in top_def.instances:
            inst_name = inst.selectpath[0]
            inst_type = inst.module_name
            modules[inst_name] = dict()

            if inst_type[:2] == 'PE':
                modules[inst_name]['type'] = 'PE'
                #modules[inst_name]['conf'] = inst.config['op'].value
                modules[inst_name]['conf'] = inst.get_config_value('op')

            elif inst_type[:5] == 'Const':
                modules[inst_name]['type'] = 'Const'
                #modules[inst_name]['conf'] = inst.config['value'].value
                modules[inst_name]['conf'] = inst.get_config_value('value')

            elif inst_type[:2] == 'IO':
                modules[inst_name]['type'] = 'IO'
                #modules[inst_name]['conf'] = inst.config['mode'].value
                modules[inst_name]['conf'] = inst.get_config_value('mode')

            elif inst_type[:3] == 'Reg':
                modules[inst_name]['type'] = 'Reg'
                modules[inst_name]['conf'] = None

            elif inst_type[:3] == 'Mem':
                modules[inst_name]['type'] = 'Mem'
                modules[inst_name]['conf'] = {
                        'mode'              : 'linebuffer', #HACK inst.get_config_value('mode'),
                        'fifo_depth'        : inst.generator_args['depth'].value,
                        'almost_full_count' : '0', #HACK
                        'chain_enable'      : '0', #HACK
                        'tile_en'           : '1', #HACK
                }

            else:
                raise ValueError("Unknown module_name '{}' expected <'PE', 'Const', 'IO', 'Reg', 'Mem'>".format(inst_type))

        ties = set()
        for con in top_module.directed_module.connections:
            src = con.source
            dst = con.sink

            src_name = src[0]
            assert src_name in modules 
            dst_name = dst[0]
            assert dst_name in modules
            src_port = _PORT_TRANSLATION[modules[src_name]['type']]['.'.join(src[1:])]
            dst_port = _PORT_TRANSLATION[modules[dst_name]['type']]['.'.join(dst[1:])]

            curr = top_def
            for select_step in src:
                curr = curr.select(select_step)

            width = curr.type.size

            tie = (src_name, src_port, dst_name, dst_port, width)
            ties.add(tie)

_PORT_TRANSLATION = {
    'PE' : {
        'data.in.0' : 'a',
        'data.in.1' : 'b',
        'data.out'  : 'pe_out_res',
        'bit.in.0'  : 'd',
        'bit.out'   : 'pe_out_p',
    },

    'Const' : {
        'out' : 'out',
    },

    'IO' : {
        'in'  : 'a',
        'out' : 'pe_out_res',
    },

    'Reg' : {
        'in'  : 'a',
        'out' : 'out',
    },

    'Mem' : {
        'rdata'  : 'mem_out',
        'addr'   : 'ain',
        'ren'    : 'ren',
        'empty'  : 'valid',
        'wdata'  : 'din',
        'wen'    : 'wen',
        'full'   : 'almost_full',
    },
}

