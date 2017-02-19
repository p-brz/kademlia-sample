#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conans import ConanFile, ConfigureEnvironment
import os
from os import path

class SampleConanFile(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    #Conan dependencies
    requires = (
        "kademlia/0.0.1-3f7a0a5@paulobrizolara/experimental",
        "clara/0.0.2.4@paulobrizolara/stable",

        "waf/0.1.1@paulobrizolara/stable",
        "WafGenerator/0.0.3@paulobrizolara/experimental"
    )

    generators = "Waf", "env", "virtualenv"

    def imports(self):
        # Copy waf executable to project folder
        self.copy("waf", dst=".")
        
        self.copy("*.dll", dst="bin", src="bin") # From bin to bin
        self.copy("*.dylib*", dst="bin", src="lib") # From lib to bin

    def build(self):
        #waf = os.path.join(".", 'waf')
        opts = self.get_options()

        self.build_path = path.abspath("build")

        env = ConfigureEnvironment(self)
        cmd = "%s waf configure build %s -o %s" % (env.command_line_env, opts, self.build_path)
        
        #self.run('%s configure build %s' % (waf, opts), cwd = self.conanfile_directory)
        self.run(cmd, cwd=self.conanfile_directory)
        
    def package(self):
        env = ConfigureEnvironment(self)
        
        # Install your project files on destination (package_folder)
        self.run("{} waf install".format(env.command_line_env))
        
    def get_options(self):
        opts = []

        if self.settings.build_type == "Debug":
            opts.append("--debug")
        else:
            opts.append("--release")

        if not hasattr(self, "package_folder") or self.package_folder is None:
            self.package_folder = path.abspath(path.join(".", "package"))
            
        opts.append("--prefix=%s" % self.package_folder)
        
        
        return " ".join(opts)
