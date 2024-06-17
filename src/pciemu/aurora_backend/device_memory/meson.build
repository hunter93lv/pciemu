pciemu_aurora_ss = ss.source_set()
pciemu_aurora_ss.add(files(
    'device_memory.c',
))
softmmu_ss.add_all(when: 'CONFIG_PCIEMU', if_true: pciemu_aurora_ss)
