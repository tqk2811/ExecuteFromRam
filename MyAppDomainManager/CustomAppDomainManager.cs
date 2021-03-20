using System;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Security.Policy;

namespace MyAppDomainManager
{
  [Guid("DF91B6ED-5152-4F65-A04A-1B1FA2B064F4"), ComVisible(true)]
  public sealed class CustomAppDomainManager : AppDomainManager, ICustomAppDomainManager
  {
    public CustomAppDomainManager()
    {
      Console.WriteLine("*** Instantiated CustomAppDomainManager");
    }

    public override void InitializeNewDomain(AppDomainSetup appDomainInfo)
    {
      Console.WriteLine("*** InitializeNewDomain");
      this.InitializationFlags = AppDomainManagerInitializationOptions.RegisterWithHost;
    }

    public override AppDomain CreateDomain(string friendlyName, Evidence securityInfo, AppDomainSetup appDomainInfo)
    {
      var appDomain = base.CreateDomain(friendlyName, securityInfo, appDomainInfo);
      Console.WriteLine("*** Created AppDomain {0}", friendlyName);
      return appDomain;
    }

    public void Run(string friendlyName, byte[] asmRaw, string[] args)
    {
      AppDomain ad = AppDomain.CreateDomain(friendlyName);
      Assembly assembly = ad.Load(asmRaw);
      if(args != null && args.Length > 0)
      {
        Console.WriteLine("C# Args: " + string.Join(" ", args));
      }
      object obj = assembly.EntryPoint.Invoke(null, args);
      AppDomain.Unload(ad);
      
      //return (obj is int code) ? code : 0;
    }
  }
}
