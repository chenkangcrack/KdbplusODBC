//==================================================================================================
///  @file QSTypeInfoMetadataSource.h
///
///  Definition of the class QSTypeInfoMetadataSource.
///
///  Copyright (C) 2008-2015 Simba Technologies Incorporated.
//==================================================================================================

#ifndef _SIMBA_QUICKSTART_QSTYPEINFOMETADATASOURCE_H_
#define _SIMBA_QUICKSTART_QSTYPEINFOMETADATASOURCE_H_

#include "Quickstart.h"

#include "DSIExtTypeInfoMetadataSource.h"

namespace Simba
{
namespace Quickstart
{
    /// @brief Quickstart sample metadata table for types supported by the DSI implementation.
    class QSTypeInfoMetadataSource : public Simba::SQLEngine::DSIExtTypeInfoMetadataSource
    {
    // Public ======================================================================================
    public:
        /// @brief Constructor.
        ///
        /// @param in_restrictions      Restrictions that may be applied to the metadata table.
        /// @param in_isODBCV3          Whether ODBC 3.X is in use
        QSTypeInfoMetadataSource(
            Simba::DSI::DSIMetadataRestrictions& in_restrictions,
            bool in_isODBCV3 = true);

        /// @brief Destructor.
        virtual ~QSTypeInfoMetadataSource();

    // Protected ===================================================================================
    protected:
        /// @brief Takes the specified type information, modifies any fields that need to be changed
        /// to fit the data-source, and indicates if that type is supported or not.
        ///
        /// This function will be called at least once for each of the basic SQL data types with
        /// io_typeInfo filled in with the default information for the SQL type. The DSII should
        /// return one of the TypePrepared values as outlined below to indicate the level of support
        /// for the specified SQL type.
        ///
        /// For example, if the data source does not support SQL_DOUBLE, it should return
        /// TP_NOT_SUPPORTED when the SqlTypeInfo representing SQL_DOUBLE is passed in.
        ///
        /// Note that the type support outlined here will not necessarily define the types that can
        /// be returned in a result set. Simba SQL Engine can return all the basic types via
        /// implicit coercion or the CONVERT() scalar function. The type support outlined here is
        /// returned via SQLGetTypeInfo(), and is mainly used to determine type support in DDL. See
        /// this page for more information:
        /// http://msdn.microsoft.com/en-us/library/ms714632%28d=lightweight,v=VS.85%29.aspx
        ///
        /// @param io_typeInfo      The default type information for a SQL type.
        ///
        /// @return One of the TypePrepared enum values:
        ///             - TP_NOT_SUPPORTED: The specified type is not supported by the data source.
        ///             - TP_DONE:          The specified type is supported by the data source, and
        ///                                 there are no more types based off this SQL type.
        ///             - TP_DUPLICATE:     The specified type is supported by the data source, and
        ///                                 there are more types based off this SQL type.
        virtual Simba::SQLEngine::TypePrepared PrepareType(Simba::SQLEngine::SqlTypeInfo& io_typeInfo);
    };
}
}

#endif
