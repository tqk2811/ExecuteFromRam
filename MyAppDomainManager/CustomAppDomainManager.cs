using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Security.Permissions;
using System.Security.Policy;

namespace MyAppDomainManager
{
  [Guid("DF91B6ED-5152-4F65-A04A-1B1FA2B064F4"), ComVisible(true)]
  public sealed class CustomAppDomainManager : AppDomainManager, ICustomAppDomainManager
  {
    public CustomAppDomainManager()
    {
      System.Diagnostics.Trace.WriteLine("*** Instantiated CustomAppDomainManager");
    }

    public override void InitializeNewDomain(AppDomainSetup appDomainInfo)
    {
      System.Diagnostics.Trace.WriteLine("*** InitializeNewDomain");
      this.InitializationFlags = AppDomainManagerInitializationOptions.RegisterWithHost;
    }

    public override AppDomain CreateDomain(string friendlyName, Evidence securityInfo, AppDomainSetup appDomainInfo)
    {
      var appDomain = base.CreateDomain(friendlyName, securityInfo, appDomainInfo);
      System.Diagnostics.Trace.WriteLine("*** Created AppDomain {0}", friendlyName);
      return appDomain;
    }


    public override Assembly EntryAssembly => assembly;//base.EntryAssembly;
    private Assembly assembly;


    public void Run(string friendlyName, string workingDir, byte[] asmRaw, string[] args)
    {
      System.Diagnostics.Trace.WriteLine("Run");
      if (string.IsNullOrEmpty(friendlyName) || string.IsNullOrEmpty(workingDir) || asmRaw == null || asmRaw.Length == 0) return;
      if (!Directory.Exists(workingDir)) return;

      AppDomain thisAD = AppDomain.CurrentDomain;

      //AppDomainSetup appDomainSetup = new AppDomainSetup();
      //appDomainSetup.ApplicationBase = workingDir;
      //appDomainSetup.PrivateBinPath = thisAD.BaseDirectory;
      AppDomain ad = thisAD;// AppDomain.CreateDomain(friendlyName,null, appDomainSetup);
      assembly = ad.Load(asmRaw);


      MethodInfo EntryPoint = assembly.EntryPoint;
      if (EntryPoint != null)
      {
        System.Diagnostics.Trace.WriteLine("EntryPoint found");
        //(new ReflectionPermission(ReflectionPermissionFlag.RestrictedMemberAccess)).Assert();
        EntryPoint.Invoke(null, null);
      }
      else
      {
        System.Diagnostics.Trace.WriteLine("EntryPoint not found");
      }
      //ad.
      //AppDomain.Unload(ad);
    }
  }
}
