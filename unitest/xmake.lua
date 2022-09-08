add_rules("mode.debug", "mode.release")

-- 开发期间默认编译debug版本，发布时默认编译release版本
set_defaultmode("debug")
--set_defaultmode("release")
set_xmakever("2.7.1")
set_languages("c++20")

add_requires("gtest", { configs = {
    main = true,
} })

target("unitest")
    set_kind("binary")
    set_default(true)
    add_files("*.cpp")
    add_packages("gtest")
target_end()