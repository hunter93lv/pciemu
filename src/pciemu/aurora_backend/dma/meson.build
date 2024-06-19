pciemu_aurora_dma_ss = ss.source_set()
pciemu_aurora_dma_ss.add(files(
    'dma.c',
))

softmmu_ss.add_all(when: 'CONFIG_PCIEMU', if_true: pciemu_aurora_dma_ss)
