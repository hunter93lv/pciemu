pciemu_aurora_common_ss = ss.source_set()
pciemu_aurora_common_ss.add(files(
    'tcp_client.c',
))
softmmu_ss.add_all(when: 'CONFIG_PCIEMU', if_true: pciemu_aurora_common_ss)
