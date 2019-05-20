package(default_visibility = ["//visibility:public"])

environment(name = "k8")

environment(name = "avr")

environment_group(
    name = "cpus",
    defaults = [
        ":k8",
    ],
    environments = [
        ":k8",
    ],
)

config_setting(
    name = "c_dbg",
    values = {
        "compilation_mode": "dbg",
    },
)

config_setting(
    name = "c_opt",
    values = {
        "compilation_mode": "opt",
    },
)
