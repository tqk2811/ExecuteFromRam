using System;
using System.Runtime.InteropServices;

namespace MyAppDomainManager
{
  [Guid("CBC92F96-2742-46C4-A7DE-FF6B60D1DC86"), ComVisible(true)]
  public interface ICustomAppDomainManager
  {
    void Run(byte[] asmRaw, string[] args);
  }
}
