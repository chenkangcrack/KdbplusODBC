Module Main
    Sub Main()
        If My.Application.CommandLineArgs.Count > 1 Then
            Return
        ElseIf My.Application.CommandLineArgs.Count = 0 Then
            CartridgeInstall.InstallMain()
        ElseIf My.Application.CommandLineArgs(0) = "-u" Then
            CartridgeUninstall.UninstallMain()
        Else
            Console.WriteLine("Incorrect options provided. Run without options to install and with '-u' to uninstall.")
        End If
    End Sub
End Module
