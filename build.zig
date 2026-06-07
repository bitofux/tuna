//!
//!  PROJECT : TUNA
//!  FILE    : build.zig
//!  AUTHOR  : bitofux
//!  DATE    : 2026-06-05
//!  BRIEF   : tuna 构建文件

const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "tuna",
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,

            .link_libc = true,
            .link_libcpp = true,
        }),
    });

    exe.root_module.addCSourceFiles(.{
        .files = &.{
            "./src/app/main.cc",
        },

        .flags = &.{ "-std=c++14", "-Wall", "-Wextra", "-Wpedantic", "-Wpedantic-errors" },
    });

    exe.root_module.addIncludePath(b.path("include"));

    exe.root_module.linkSystemLibrary("pthread", .{});

    const install_exe = b.addInstallArtifact(exe, .{});
    b.getInstallStep().dependOn(&install_exe.step);

    const run_cmd = b.addRunArtifact(exe);

    run_cmd.step.dependOn(&install_exe.step);

    const run_step = b.step("run", "Run tuna server");
    run_step.dependOn(&run_cmd.step);
}
