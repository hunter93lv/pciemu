pciemu_glue_ss = ss.source_set()
pciemu_glue_ss.add(files(
    'glue.c',
))
softmmu_ss.add_all(when: 'CONFIG_PCIEMU', if_true: pciemu_glue_ss)
