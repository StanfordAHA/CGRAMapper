import coreir

def test_load_core(libs, files):
    print()
    for file in files:
        context = coreir.Context()
        for lib in libs:
            context.load_library(lib)
        top_module = context.load_from_file(file)
        assert top_module is not None
        top_def = top_module.definition
        assert top_def is not None
        modules = dict()

        for inst in top_def.instances:
            inst_name = inst.selectpath[0]
            inst_type = inst.module.name
            modules[inst_name] = dict()

            if inst_type == 'PE':
                modules[inst_name]['type'] = 'PE'

                op_kind = inst.module.generator_args['op_kind'].value 

                if op_kind in ('alu', 'combined'):
                    modules[inst_name]['alu_op'] = inst.config['alu_op'].value

                if op_kind in ('bit', 'combined'):
                    modules[inst_name]['lut_value'] = inst.config['lut_value'].value

                if op_kind not in ('bit', 'alu', 'combined'):
                    raise ValueError("Unkown op_kind `{}' in `{}' expected <`bit', `data', `combined'>".format(file, op_kind))

            elif inst_type == 'IO':
                modules[inst_name]['type'] = 'IO'
                modules[inst_name]['conf'] = conf = inst.config['mode'].value
                assert conf in ('in', 'out') 
            elif inst_type == 'BitIO':
                modules[inst_name]['type'] = 'BitIO'
                modules[inst_name]['conf'] = conf =  inst.config['mode'].value
                assert conf in ('in', 'out') 
            elif inst_type == 'reg':
                modules[inst_name]['type'] = 'Reg'
                modules[inst_name]['conf'] = None
            elif inst_type == 'dff':
                modules[inst_name]['type'] = 'BitReg'
                modules[inst_name]['conf'] = None
            elif inst_type == 'Mem':
                ''' 
                expected memory conf
                conf = {
                    'mode'        :: ('linebuffer', 'fifo', 'ram')
                    'depth'       :: INT
                    'full_count'  :: INT
                    'tile_en'     :: 1
                    'chain_enable :: 0
                }
                '''
                    
                modules[inst_name]['type'] = 'Mem'
                modules[inst_name]['conf'] = conf = {
                        'mode'         : inst.config['mode'].value,
                        'depth'        : inst.config['depth'].value,
                        'full_count'   : inst.config['full_count'].value,
                        'tile_en'      : inst.config['tile_en'].value,
                        'chain_enable' : inst.config['chain_enable'].value,
                }
                assert conf['mode'] in ('linebuffer', 'fifo', 'ram')
                assert isinstance(conf['depth'], int)
                assert isinstance(conf['full_count'], int)
                assert conf['tile_en'] == 1
                assert conf['chain_enable'] == 0

            elif inst_type == 'const':
                modules[inst_name]['type'] = 'Const'
                modules[inst_name]['conf'] = inst.config['value'].value

            else:
                raise ValueError("Unknown module_name `{}' in `{}' expected <`PE', `DataPE', `BitPE', `Const', `IO', `BitIO',  `Reg', `Mem'>".format(inst_type, file))

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

        print("{}: PASSED".format(file))

_PORT_TRANSLATION = {
    'PE' : {
        'data.in.0' : 'a',
        'data.in.1' : 'b',
        'data.out'  : 'pe_out_res',
        'bit.in.0'  : 'd',
        'bit.in.1'  : 'e',
        'bit.in.2'  : 'f',
        'bit.out'   : 'pe_out_p',
    },
    
    'Const' : {
        'out' : 'out',
    },

    'IO' : {
        'in'  : 'a',
        'out' : 'pe_out_res',
    },

    'BitIO' : {
        'in'  : 'd',
        'out' : 'pe_out_p',
    },

    'Reg' : {
        'in'  : 'a',
        'out' : 'out',
    },
    
    'BitReg' : {
        'in'  : 'd',
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

