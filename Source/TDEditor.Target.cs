// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using UnrealBuildTool;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

public class TDEditorTarget : TargetRules
{
	public TDEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "TD" } );

        IncrementalBuildNumber(Target);
    }


    #region Incremental Build
    const string MODULE_NAME = "TD";

    private void IncrementalBuildNumber(TargetInfo Target) {
        string PROJECT_PATH = this.ProjectFile.Directory.FullName;
        string GAME_VERSION_FILE = PROJECT_PATH + "/Source/" + MODULE_NAME + "/Public/GameVersion.generated.h";
        string GAME_META_FILE = PROJECT_PATH + "/Source/" + MODULE_NAME + ".Meta.ini";

        bool bMetaFile = File.Exists(GAME_META_FILE);
        MetaInformation meta = new MetaInformation();

        if (bMetaFile) {
            meta.Read(GAME_META_FILE);
        } else {
            meta.Write(GAME_META_FILE + ".generated");
            throw new BuildException("Failed to read meta file, generated default at " + GAME_META_FILE + ".generated");
        }

        string gitHash = getGitHash();

        // Increment build number and set date
        meta.BuildNumber++;
        meta.BuildDate = DateTime.Now.ToString("dddd, dd MMMM yyyy");
        meta.BuildTime = DateTime.Now.ToString("HH:mm:ss");
        meta.GitHash = gitHash;
        meta.Write(GAME_META_FILE);


        System.Console.WriteLine(
            "New build number generated: "
            + meta.BuildNumber + " @ " + meta.BuildDate + " - " + meta.BuildTime
            + " # " + gitHash);

        // Write the header file
        string headerFileGenerated =
            "/** DO NOT MODIFY THIS FILE, ITS IS AUTOMATICALLY GENERATED ON EACH BUILD! **/"
            + "\n"
            + "\n"
            + "// Version numbers\n"
            + "#define BUILD_MAJOR_VERSION " + meta.Major + "\n"
            + "#define BUILD_MINOR_VERSION " + meta.Minor + "\n"
            + "#define BUILD_PATCH_VERSION " + meta.Patch + "\n"
            + "\n"
            + "// Build\n"
            + "#define BUILD_BUILD_NUMBER " + meta.BuildNumber + "\n"
            + "#define BUILD_BUILD_DATE \"" + meta.BuildDate + "\"\n"
            + "#define BUILD_BUILD_TIME \"" + meta.BuildTime + "\"\n"
            + "\n"
            + "// Git\n"
            + "#define BUILD_GIT_HASH \"" + gitHash + "\"\n"
        ;

        System.IO.File.WriteAllText(GAME_VERSION_FILE, headerFileGenerated);
    }

    private string getGitHash() {
        string output = "";

        var p = new Process {
            StartInfo = new ProcessStartInfo {
                FileName = "git",
                Arguments = "rev-parse --short HEAD",
                UseShellExecute = false,
                RedirectStandardOutput = true,
                CreateNoWindow = true,
                WorkingDirectory = this.ProjectFile.Directory.FullName
            }
        };

        p.Start();
        while (!p.StandardOutput.EndOfStream)
            output += p.StandardOutput.ReadLine() + "\n";


        return output.Remove(output.Length - 1);
    }

    class MetaInformation {
        public uint Major = 0;
        public uint Minor = 1;
        public uint Patch = 0;

        public uint BuildNumber;
        public string BuildDate;
        public string BuildTime;
        public string GitHash;

        public void Read(string file) {
            var i = new IniFile(file);

            var _major = i.Read("Major", "Version");
            var _minor = i.Read("Minor", "Version");
            var _patch = i.Read("Patch", "Version");

            var _bn = i.Read("BuildNumber", "Build");
            BuildDate = i.Read("BuildDate", "Build");
            BuildTime = i.Read("BuildDate", "Build");


            try { uint.TryParse(_major, out Major); } catch (Exception) { throw new BuildException("Failed to parse Major number (" + _major + ")"); }
            try { uint.TryParse(_minor, out Minor); } catch (Exception) { throw new BuildException("Failed to parse Minor number (" + _minor + ")"); }
            try { uint.TryParse(_patch, out Patch); } catch (Exception) { throw new BuildException("Failed to parse Patch number (" + _patch + ")"); }
            try { uint.TryParse(_bn, out BuildNumber); } catch (Exception) { throw new BuildException("Failed to parse BuildNumber number (" + _bn + ")"); }
        }

        public void Write(string file) {
            var i = new IniFile(file);

            i.Write("Major", "" + Major, "Version");
            i.Write("Minor", "" + Minor, "Version");
            i.Write("Patch", "" + Patch, "Version");

            i.Write("BuildNumber", "" + BuildNumber, "Build");
            i.Write("BuildDate", "" + BuildDate, "Build");
            i.Write("BuildTime", "" + BuildTime, "Build");
            i.Write("GitHash", "" + GitHash, "Build");
        }
    }

    // revision 11
    class IniFile {
        string Path;

        [DllImport("kernel32", CharSet = CharSet.Unicode)]
        static extern long WritePrivateProfileString(string Section, string Key, string Value, string FilePath);

        [DllImport("kernel32", CharSet = CharSet.Unicode)]
        static extern int GetPrivateProfileString(string Section, string Key, string Default, StringBuilder RetVal, int Size, string FilePath);

        public IniFile(string IniPath = null) {
            Path = new FileInfo(IniPath ?? "Default.ini").FullName.ToString();
        }

        public string Read(string Key, string Section = null) {
            var RetVal = new StringBuilder(255);
            GetPrivateProfileString(Section ?? "Default", Key, "", RetVal, 255, Path);
            return RetVal.ToString();
        }

        public void Write(string Key, string Value, string Section = null) {
            WritePrivateProfileString(Section ?? "Default", Key, Value, Path);
        }

        public void DeleteKey(string Key, string Section = null) {
            Write(Key, null, Section ?? "Default");
        }

        public void DeleteSection(string Section = null) {
            Write(null, null, Section ?? "Default");
        }

        public bool KeyExists(string Key, string Section = null) {
            return Read(Key, Section).Length > 0;
        }
    }
    #endregion

}
