pciemu_front_end_ss = ss.source_set()
pciemu_front_end_ss.add(files(
    'irq.c',
    'regbar.c',
    'membar.c',
    'pciemu.c',
    'internal_bar.c',
))
softmmu_ss.add_all(when: 'CONFIG_PCIEMU', if_true: pciemu_front_end_ss)
