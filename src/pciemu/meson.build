pciemu_ss = ss.source_set()
pciemu_ss.add(files(
    'dma.c',
    'irq.c',
    'regbar.c',
    'membar.c',
    'pciemu.c',
))
subdir('aurora_backend')
softmmu_ss.add_all(when: 'CONFIG_PCIEMU', if_true: pciemu_ss)
