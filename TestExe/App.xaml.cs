using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

namespace TestExe
{
  /// <summary>
  /// Interaction logic for App.xaml
  /// </summary>
  public partial class App : Application
  {
    private void Application_Startup(object sender, StartupEventArgs e)
    {
      var args = e.Args;
      System.Diagnostics.Trace.WriteLine("Application_Startup Args.Count: " + args.Length);
      MainWindow window = new MainWindow();
      window.Show();
    }
  }
}
