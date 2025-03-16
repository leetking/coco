add_requires(
    "glib-2.0",
    {system = true}
)

set_project("coco")
set_version("0.0.2")

target("co_io")
    set_kind("static")
    set_languages("gnu11")
    add_files("co_io.c")
    add_deps("co")

target("co")
    set_kind("static")
    set_languages("c11")
    add_files("co.c")
    add_deps("co_ctx")

target("co_ctx")
    set_kind("static")
    set_languages("c11")
    add_files("co_make_ctx.s", "co_jump_ctx.s")

-- test target
target("co_ctx_test")
    set_kind("binary")
    set_languages("c11")
    add_rules("mode.debug", "mode.release")
    add_files("co_ctx_test.c")
    add_deps("co_ctx")

target("co_test")
    set_kind("binary")
    set_languages("c11")
    add_rules("mode.debug", "mode.release")
    add_files("co_test.c")
    add_deps("co")

target("echosvr")
    set_kind("binary")
    set_languages("gnu11")
    add_rules("mode.debug", "mode.release")
    add_files("echosvr.c")
    add_deps("co_io")

target("press_tool")
    set_kind("binary")
    set_languages("gnu11")
    add_rules("mode.debug", "mode.release")
    add_files("press_tool.c")
    add_deps("co_io")
    add_packages("glib-2.0")
