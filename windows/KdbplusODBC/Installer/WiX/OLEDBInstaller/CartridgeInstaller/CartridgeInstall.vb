Module CartridgeInstall

    Private Sub CopyCartridgeToPath(path As String)
        On Error Resume Next
        If (My.Computer.FileSystem.DirectoryExists(path)) Then
            My.Computer.FileSystem.CopyFile("quickstart.xsl", path + "\quickstart.xsl")
        End If
    End Sub

    Private Sub CopyCartridgeToInstances(names As List(Of String), isNative As Boolean)
        For Each name As String In names
            If isNative Then
                CopyCartridgeToPath("C:\Program Files\Microsoft SQL Server\" + name + "\OLAP\bin\Cartridges")
            Else
                CopyCartridgeToPath("C:\Program Files (x86)\Microsoft SQL Server\" + name + "\OLAP\bin\Cartridges")
            End If
        Next
    End Sub

    Private Sub CopyCartridgeToSQLServer(vers As List(Of String), isNative As Boolean)
        For Each ver As String In vers
            If isNative Then
                If CInt(ver) > 100 Then
                    CopyCartridgeToPath("C:\Program Files\Microsoft SQL Server\" + ver + "\Tools\Binn\ManagementStudio\DataWarehouseDesigner\UIRdmsCartridge")
                Else
                    CopyCartridgeToPath("C:\Program Files\Microsoft SQL Server\" + ver + "\Tools\Binn\VSShell\Common7\IDE\DataWarehouseDesigner\UIRdms\Cartridge")
                End If
            Else
                If CInt(ver) > 100 Then
                    CopyCartridgeToPath("C:\Program Files (x86)\Microsoft SQL Server\" + ver + "\Tools\Binn\ManagementStudio\DataWarehouseDesigner\UIRdmsCartridge")
                Else
                    CopyCartridgeToPath("C:\Program Files (x86)\Microsoft SQL Server\" + ver + "\Tools\Binn\VSShell\Common7\IDE\DataWarehouseDesigner\UIRdms\Cartridge")
                End If
            End If
        Next
    End Sub

    Private Sub CopyCartridgeToAnalysisServices(vers As List(Of String), isNative As Boolean)
        For Each ver As String In vers
            If isNative Then
                If CInt(ver) = 100 Then
                    CopyCartridgeToPath("C:\Program Files\Microsoft Analysis Services\AS OLEDB\10\Cartridges")
                End If
                CopyCartridgeToPath("C:\Program Files\Microsoft Analysis Services\AS OLEDB\" + ver + "\Cartridges")
            End If

            If Environment.Is64BitOperatingSystem Then
                If CInt(ver) = 100 Then
                    CopyCartridgeToPath("C:\Program Files (x86)\Microsoft Analysis Services\AS OLEDB\10\Cartridges")
                End If
                CopyCartridgeToPath("C:\Program Files (x86)\Microsoft Analysis Services\AS OLEDB\" + ver + "\Cartridges")
            End If
        Next
    End Sub

    Private Sub CopyCartridgeToSSDT(vers As List(Of String))
        For Each ver In vers
            If Environment.Is64BitOperatingSystem Then
                If CInt(ver) > 100 Then
                    CopyCartridgeToPath("C:\Program Files (x86)\Microsoft Visual Studio " + ver + "\Common7\IDE\PrivateAssemblies\Business Intelligence Semantic Model\Cartridges")
                End If

                CopyCartridgeToPath("C:\Program Files (x86)\Microsoft Visual Studio " + ver + "\Common7\IDE\PrivateAssemblies\DataWarehouseDesigner\UIRdmsCartridge")
            Else
                If CInt(ver) > 100 Then
                    CopyCartridgeToPath("C:\Program Files\Microsoft Visual Studio " + ver + "\Common7\IDE\PrivateAssemblies\Business Intelligence Semantic Model\Cartridges")
                End If

                CopyCartridgeToPath("C:\Program Files\Microsoft Visual Studio " + ver + "\Common7\IDE\PrivateAssemblies\DataWarehouseDesigner\UIRdmsCartridge")

            End If
        Next
    End Sub

    Public Sub InstallMain()
        MsgBox("testing")
        Dim sqlServerVers = Versions.GetSqlServerVersions()

        CopyCartridgeToAnalysisServices(sqlServerVers, True)
        CopyCartridgeToInstances(Versions.GetNativeMSASInstances(), True)
        CopyCartridgeToSQLServer(sqlServerVers, True)

        If (Environment.Is64BitOperatingSystem) Then
            CopyCartridgeToAnalysisServices(sqlServerVers, False)
            CopyCartridgeToInstances(Versions.Get32BitMSASInstances(), False)
            CopyCartridgeToSQLServer(sqlServerVers, False)
        End If

        CopyCartridgeToSSDT(Versions.GetVisualStudioVersions())
    End Sub

End Module

