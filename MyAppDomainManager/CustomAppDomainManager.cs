using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Security.Permissions;
using System.Security.Policy;
using System.Linq;

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

      AppDomain thisAD = AppDomain.CurrentDomain;
      if(args != null && args.Length > 0) thisAD.SetupInformation.AppDomainInitializerArguments = args;
      assembly = thisAD.Load(asmRaw);
      MethodInfo EntryPoint = assembly.EntryPoint;
      if (EntryPoint != null)
      {
        var paras = EntryPoint.GetParameters();
        if(paras.Length > 0 && paras.First().ParameterType == typeof(string[]))
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
  }
}
