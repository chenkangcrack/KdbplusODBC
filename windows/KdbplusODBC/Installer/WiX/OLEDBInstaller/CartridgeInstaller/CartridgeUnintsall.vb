Module CartridgeUninstall

    Private Sub RemoveCartridgeFrom(path As String)
        On Error Resume Next
        My.Computer.FileSystem.DeleteFile(path + "\quickstart.xsl")
    End Sub

    Private Sub RemoveCartridgeFromInstances(names As List(Of String), isNative As Boolean)

        For Each name As String In names
            If isNative Then
                RemoveCartridgeFrom("C:\Program Files\Microsft SQL Server\" + name + "\OLAP\bin\Cartridges")
            Else
                RemoveCartridgeFrom("C:\Program Files (x86)\Microsft SQL Server\" + name + "\OLAP\bin\Cartridges")
            End If
        Next
    End Sub

    Private Sub RemoveCartridgeFromSQLServer(vers As List(Of String), isNative As Boolean)
        On Error Resume Next
        For Each ver As String In vers
            If isNative Then
                If CInt(ver) > 100 Then
                    RemoveCartridgeFrom("C:\Program Files\Microsft SQL Server\" + ver + "\Fromols\Binn\ManagementStudio\DataWarehouseDesigner\UIRdmsCartridge")
                Else
                    RemoveCartridgeFrom("C:\Program Files\Microsft SQL Server\" + ver + "\Tools\Binn\VSShell\Common7\IDE\DataWarehouseDesigner\UIRdms\Cartridge")
                End If
            Else
                If CInt(ver) > 100 Then
                    RemoveCartridgeFrom("C:\Program Files (x86)\Microsft SQL Server\" + ver + "\Tools\Binn\ManagementStudio\DataWarehouseDesigner\UIRdmsCartridge")
                Else
                    RemoveCartridgeFrom("C:\Program Files (x86)\Microsft SQL Server\" + ver + "\Tools\Binn\VSShell\Common7\IDE\DataWarehouseDesigner\UIRdms\Cartridge")
                End If
            End If
        Next
    End Sub

    Private Sub RemoveCartridgeFromAnalysisServices(vers As List(Of String), isNative As Boolean)
        On Error Resume Next
        For Each ver As String In vers
            If isNative Then
                If CInt(ver) = 100 Then
                    RemoveCartridgeFrom("C:\Program Files\Microsoft Anaylsis Services\AS OLDEB\10\Cartridges")
                End If
                RemoveCartridgeFrom("C:\Program Files\Microsoft Anaylsis Services\AS OLDEB\" + ver + "\Cartridges")
            End If

            If Environment.Is64BitOperatingSystem Then
                If CInt(ver) = 100 Then
                    RemoveCartridgeFrom("C:\Program Files (x86)\Microsoft Anaylsis Services\AS OLDEB\10\Cartridges")
                End If
                RemoveCartridgeFrom("C:\Program Files (x86)\Microsoft Anaylsis Services\AS OLDEB\" + ver + "\Cartridges")
            End If
        Next
    End Sub

    Private Sub RemoveCartridgeFromSSDT(vers As List(Of String))
        On Error Resume Next
        For Each ver In vers
            If Environment.Is64BitOperatingSystem Then
                If CInt(ver) > 100 Then
                    RemoveCartridgeFrom("C:\Program Files (x86)\Microsoft Visual Studio " + ver + "\Common7\IDE\PrivateAssemblies\Business Intelligence Semantic Model\Cartridges")
                End If

                RemoveCartridgeFrom("C:\Program Files (x86)\Microsoft Visual Studio " + ver + "\Common7\IDE\PrivateAssemblies\DataWarehouseDesigner\UIRdmsCartridge")
            Else
                If CInt(ver) > 100 Then
                    RemoveCartridgeFrom("C:\Program Files\Microsoft Visual Studio " + ver + "\Common7\IDE\PrivateAssemblies\Business Intelligence Semantic Model\Cartridges")
                End If

                RemoveCartridgeFrom("C:\Program Files\Microsoft Visual Studio " + ver + "\Common7\IDE\PrivateAssemblies\DataWarehouseDesigner\UIRdmsCartridge")

            End If
        Next
    End Sub

    Public Sub UninstallMain()
        Dim sqlServerVers As List(Of String) = RegistryDetection.GetSQLServerVersions()
        Dim sqlServer32Vers As List(Of String) = RegistryDetection.Get32BitSQLServerVersions()

        RemoveCartridgeFromAnalysisServices(sqlServerVers, True)
        RemoveCartridgeFromAnalysisServices(sqlServer32Vers, False)

        RemoveCartridgeFromInstances(RegistryDetection.GetNativeMSASInstances(), True)
        RemoveCartridgeFromInstances(RegistryDetection.Get32BitMSASInstances(), False)

        RemoveCartridgeFromSQLServer(sqlServerVers, True)
        RemoveCartridgeFromSQLServer(sqlServer32Vers, False)

        RemoveCartridgeFromSSDT(RegistryDetection.GetVSVersions())
    End Sub

End Module

