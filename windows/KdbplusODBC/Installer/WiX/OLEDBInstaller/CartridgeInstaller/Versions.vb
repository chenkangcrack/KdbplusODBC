Module Versions
    Public Function GetSqlServerVersions() As List(Of String)
        Return New List(Of String) From {"90", "100", "110", "120"}
    End Function

    Public Function GetVisualStudioVersions() As List(Of String)
        Return New List(Of String) From {"8.0", "9.0", "10.0", "11.0", "12.0", "13.0"}
    End Function

    Public Function GetNativeMSASInstances() As List(Of String)
        Dim instances As New List(Of String)
        Dim microsoft = My.Computer.Registry.LocalMachine.OpenSubKey("SOFTWARE").OpenSubKey("Microsoft")

        If Not microsoft Is Nothing Then
            Dim sqlServer = microsoft.OpenSubKey("Microsoft SQL Server")

            If Not sqlServer Is Nothing Then
                Dim instanceNames = sqlServer.OpenSubKey("InstanceNames")

                If Not instanceNames Is Nothing Then
                    Dim olap = instanceNames.OpenSubKey("OLAP")

                    If Not olap Is Nothing Then
                        Dim valueNames = olap.GetValueNames

                        Dim index As Integer
                        For index = 0 To valueNames.Length
                            instances.Add(olap.GetValue(valueNames(index)))
                        Next index
                    End If
                End If
            End If
        End If

        Return instances
    End Function

    Public Function Get32BitMSASInstances() As List(Of String)
        Dim instances As New List(Of String)
        Dim wow6432node = My.Computer.Registry.LocalMachine.OpenSubKey("SOFTWARE").OpenSubKey("Wow6432Node")

        If Not wow6432node Is Nothing Then
            Dim microsoft = wow6432node.OpenSubKey("Microsoft")

            If Not microsoft Is Nothing Then
                Dim sqlServer = microsoft.OpenSubKey("Microsoft SQL Server")

                If Not sqlServer Is Nothing Then
                    Dim instanceNames = sqlServer.OpenSubKey("InstanceNames")

                    If Not instanceNames Is Nothing Then
                        Dim olap = instanceNames.OpenSubKey("OLAP")

                        If Not olap Is Nothing Then
                            Dim valueNames = olap.GetValueNames

                            Dim index As Integer
                            For index = 0 To valueNames.Length
                                instances.Add(olap.GetValue(valueNames(index)))
                            Next index
                        End If
                    End If
                End If
            End If
        End If

        Return instances
    End Function
End Module
