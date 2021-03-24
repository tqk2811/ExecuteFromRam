using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace TestExe
{
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {
    public string[] args;
    public MainWindow()
    {
      Console.WriteLine(System.Runtime.InteropServices.RuntimeInformation.FrameworkDescription);
      InitializeComponent();
    }

    private void Window_Loaded(object sender, RoutedEventArgs e)
    {
      MessageBox.Show(string.Join(",", Environment.GetCommandLineArgs()),"Args");
      using(StreamWriter sw = new StreamWriter("test.txt"))
      {
        sw.WriteLine(Guid.NewGuid().ToString());
      }
      MessageBox.Show(File.ReadAllText("test.txt"), "text");
    }
  }
}
