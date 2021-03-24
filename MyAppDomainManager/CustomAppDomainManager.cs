using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Security.Permissions;
using System.Security.Policy;
using System.Linq;
using System.Security;
using System.Security.AccessControl;

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


    public void Run(byte[] asmRaw, string[] args)
    {
      System.Diagnostics.Trace.WriteLine("Run");
      if (asmRaw == null || asmRaw.Length == 0) return;

      RunInCurrentDomain(asmRaw, args);
      //RunInNewDomain(asmRaw, args);
    }

    void RunInCurrentDomain(byte[] asmRaw, string[] args)
    {
      AppDomain ad = AppDomain.CurrentDomain;
      assembly = ad.Load(asmRaw);

      MethodInfo EntryPoint = assembly.EntryPoint;
      if (EntryPoint != null)
      {
        var paras = EntryPoint.GetParameters();
        if (paras.Length > 0 && paras.First().ParameterType == typeof(string[]))
        {
          EntryPoint.Invoke(null, new object[] { args });//Main(string[] args)
        }
        else EntryPoint.Invoke(null, null);
      }
      else
      {
        System.Diagnostics.Trace.WriteLine("EntryPoint not found");
      }
    }

    void RunInNewDomain(byte[] asmRaw, string[] args)
    {
      AppDomain thisAD = AppDomain.CurrentDomain;
      AppDomainSetup appDomainSetup = new AppDomainSetup();
      //appDomainSetup.AppDomainInitializer = new AppDomainInitializer(ChildDomainInit);
      appDomainSetup.AppDomainInitializerArguments = args;
      appDomainSetup.ApplicationBase = thisAD.SetupInformation.ApplicationBase;

      PermissionSet permSet = new PermissionSet(PermissionState.None);
      permSet.AddPermission(new SecurityPermission(SecurityPermissionFlag.Execution));
      permSet.AddPermission(new UIPermission(PermissionState.Unrestricted));
      permSet.AddPermission(new FileIOPermission(FileIOPermissionAccess.PathDiscovery, AccessControlActions.View, appDomainSetup.ApplicationBase));
      permSet.AddPermission(new FileIOPermission(FileIOPermissionAccess.Read, AccessControlActions.View, appDomainSetup.ApplicationBase));

      AppDomain ad = AppDomain.CreateDomain(Guid.NewGuid().ToString(), thisAD.Evidence, appDomainSetup, permSet);

      assembly = ad.Load(asmRaw);
      MethodInfo EntryPoint = assembly.EntryPoint;
      if (EntryPoint != null)
      {
        var paras = EntryPoint.GetParameters();
        if (paras.Length > 0 && paras.First().ParameterType == typeof(string[]))
        {
          EntryPoint.Invoke(null, new object[] { args });//Main(string[] args)
        }
        else EntryPoint.Invoke(null, null);
      }
      else
      {
        System.Diagnostics.Trace.WriteLine("EntryPoint not found");
      }
      AppDomain.Unload(ad);
    }
  }
}
