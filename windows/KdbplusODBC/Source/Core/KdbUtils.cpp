#define KXVER 3
#include <bcon.h>
#include <QSRecord.h>
#include <stdio.h>
#include <stdlib.h>
#include"k.h"

bool initialised = false;
QSDatac_client_t *client;
char *database;

bool is_oid(char* g) { return (g[0]=='\0')&&(g[1]=='\0')&&(g[2]=='\0')&&(g[3]=='\0'); }
U oid_to_guid(const QSRecord_oid_t* o) {
  U u;
  u.g[0]='\0';u.g[1]='\0';u.g[2]='\0';u.g[3]='\0';
  memcpy(&u.g[4],o->bytes,sizeof(QSRecord_oid_t));
  return u;
}


void append_row_item(QSRecord_t* doc, S cc, K cd, int i) {
  QSRecord_oid_t oid;
  switch (cd->t) {
      case  0: append_item(doc, cc, kK(cd)[i]); break;
      case  1: QSRecord_append_bool(doc, cc, -1, !0==kG(cd)[i]); break;
      case  2: if (is_oid((char*)kU(cd)[i].g)) {
                 QSRecord_oid_init_from_data(&oid,&kU(cd)[i].g[4]);
                 QSRecord_append_oid(doc, cc, -1, &oid);
	             } else {
                 QSRecord_append_binary (doc, cc, -1, QSRecord_SUBTYPE_UUID,kU(cd)[i].g,sizeof(U)); 
	             } ; break;
      case  4: QSRecord_append_int32(doc, cc, -1, kG(cd)[i]); break;
      case  5: QSRecord_append_int32(doc, cc, -1, kH(cd)[i]); break;
      case  6: QSRecord_append_int32(doc, cc, -1, kI(cd)[i]); break; 
      case  7: QSRecord_append_int64(doc, cc, -1, kJ(cd)[i]); break; 
      case  8: QSRecord_append_double(doc, cc, -1, kE(cd)[i]); break; 
      case  9: QSRecord_append_double(doc, cc, -1, kF(cd)[i]); break; 
      case 10: QSRecord_append_utf8(doc, cc, -1, &kC(cd)[i], 1); break; 
      case 11: QSRecord_append_utf8(doc, cc, -1, kS(cd)[i], -1); break;
      case 12: QSRecord_append_date_time(doc, cc, -1, 946684800000+kJ(cd)[i]/1000000); break;
      case 13: case 14: case 17: case 18: case 19:
               QSRecord_append_int32(doc, cc, -1, kI(cd)[i]); break; 
      case 15: QSRecord_append_date_time(doc, cc, -1, 946684800000+kF(cd)[i]*86400000); break;
      case 16: QSRecord_append_int64(doc, cc, -1, kJ(cd)[i]); break; 
    }

}

void table_to_QSRecord(QSRecord_t* doc, K table) {
  int rc,j; uint32_t i; const char *key; char str[16];
  QSRecord_t child; K cols, data;
  cols = kK(table->k)[0];
  data = kK(table->k)[1];
  rc = kK(data)[0]->n;
  for(i=0;i<rc;i++) {
    QSRecord_uint32_to_string (i, &key, str, sizeof str);
    QSRecord_append_array_begin(doc, key, -1, &child); 
    for(j=0;j<cols->n;j++) append_row_item(&child, kS(cols)[j], kK(data)[j], i);
    QSRecord_append_array_end(doc, &child);
  } 
}

void list_to_QSRecord(QSRecord_t* doc, K list) {
  uint32_t i; const char *key; char str[16]; QSRecord_oid_t oid;
  switch (list->t) {
    case 0: for(i=0;i<list->n;i++){ QSRecord_uint32_to_string (i, &key, str, sizeof str); append_item(doc, key, kK(list)[i]); } break;
    case 1: for(i=0;i<list->n;i++){ QSRecord_uint32_to_string (i, &key, str, sizeof str); QSRecord_append_bool (doc, key, -1, !0==kG(list)[i]); } break;
    case 2: for(i=0;i<list->n;i++) {
        QSRecord_uint32_to_string (i, &key, str, sizeof str);
        if (is_oid((char*)kU(list)[i].g)) {
          QSRecord_oid_init_from_data(&oid,&kU(list)[i].g[4]);
          QSRecord_append_oid(doc, key, -1, &oid);
  	    } else {
          QSRecord_append_binary (doc, key, -1, QSRecord_SUBTYPE_UUID,kU(list)[i].g,sizeof(U)); 
  	    } ;
      } ; break;
    case 5: for(i=0;i<list->n;i++){ QSRecord_uint32_to_string (i, &key, str, sizeof str); QSRecord_append_int32(doc, key, -1, kH(list)[i]); } break;
    case 6: for(i=0;i<list->n;i++){ QSRecord_uint32_to_string (i, &key, str, sizeof str); QSRecord_append_int32(doc, key, -1, kI(list)[i]); } break;
    case 7: for(i=0;i<list->n;i++){ QSRecord_uint32_to_string (i, &key, str, sizeof str); QSRecord_append_int64(doc, key, -1, kJ(list)[i]); } break;
    case 8: for(i=0;i<list->n;i++){ QSRecord_uint32_to_string (i, &key, str, sizeof str); QSRecord_append_double(doc, key, -1, kE(list)[i]); } break;
    case 9: for(i=0;i<list->n;i++){ QSRecord_uint32_to_string (i, &key, str, sizeof str); QSRecord_append_double(doc, key, -1, kF(list)[i]); } break;
    case 11: for(i=0;i<list->n;i++){ QSRecord_uint32_to_string (i, &key, str, sizeof str); QSRecord_append_utf8(doc, key, -1, kS(list)[i], -1); } break;
    case 12: for(i=0;i<list->n;i++){ QSRecord_uint32_to_string (i, &key, str, sizeof str); QSRecord_append_date_time(doc, key, -1, 946684800000+kJ(list)[i]/1000000); } break;
    case 13: case 14: case 17: case 18: case 19:
      for(i=0;i<list->n;i++){ QSRecord_uint32_to_string (i, &key, str, sizeof str); QSRecord_append_int32(doc, key, -1, kI(list)[i]); } break;
    case 15: for(i=0;i<list->n;i++){ QSRecord_uint32_to_string (i, &key, str, sizeof str); QSRecord_append_date_time(doc, key, -1, 946684800000+kF(list)[i]*86400000); } break;
    case 16: for(i=0;i<list->n;i++){ QSRecord_uint32_to_string (i, &key, str, sizeof str); QSRecord_append_int64(doc, key, -1, kJ(list)[i]); } break;
  }
}


void append_item(QSRecord_t* doc, const char* strkey, K item) {
  K binary; QSRecord_oid_t oid; QSRecord_t child;
  switch (item->t) {
    case  -1: QSRecord_append_bool  (doc, strkey, -1, !0==item->g); break;
    case  -2: 
      if(is_oid((char*)item->G0)) {
        QSRecord_oid_init_from_data(&oid,&((char*)item->G0)[4]);
        QSRecord_append_oid(doc, strkey, -1, &oid);
      } else {
        QSRecord_append_binary(doc, strkey, -1, QSRecord_SUBTYPE_UUID, kU(item)->g, sizeof(U));
      }
      break;
    case  -4: QSRecord_append_int32 (doc, strkey, -1, item->g); break;
    case  -5: QSRecord_append_int32 (doc, strkey, -1, item->h); break;
    case  -6: QSRecord_append_int32 (doc, strkey, -1, item->i); break;
    case  -7: QSRecord_append_int64 (doc, strkey, -1, item->j); break;
    case  -8: QSRecord_append_double(doc, strkey, -1, item->e); break;
    case  -9: QSRecord_append_double(doc, strkey, -1, item->f); break;
    case -10: QSRecord_append_utf8  (doc, strkey, -1, &item->g, 1); break;
    case -11: QSRecord_append_utf8  (doc, strkey, -1, item->s, -1); break;
    case -12: QSRecord_append_date_time  (doc, strkey, -1, 946684800000+(item->j)/1000000); break;
    case -13: QSRecord_append_int32 (doc, strkey, -1, item->i); break;
    case -14: QSRecord_append_int32 (doc, strkey, -1, item->i); break;
    case -15: QSRecord_append_date_time  (doc, strkey, -1, 946684800000+(item->f)*86400000); break;
    case -16: QSRecord_append_int64 (doc, strkey, -1, item->j); break;
    case -17: QSRecord_append_int32 (doc, strkey, -1, item->i); break;
    case -18: QSRecord_append_int32 (doc, strkey, -1, item->i); break;
    case -19: QSRecord_append_int32 (doc, strkey, -1, item->i); break;
    case   4: QSRecord_append_binary(doc, strkey, -1, QSRecord_SUBTYPE_BINARY, kG(item), item->n); break;
    case  10: QSRecord_append_utf8  (doc, strkey, -1, kC(item), item->n); break;
    case  99:
      QSRecord_append_document_begin(doc, strkey, -1, &child); 
      dict_to_QSRecord(&child,item);
      QSRecord_append_document_end(doc, &child);
      break;
    case  98:
      QSRecord_append_array_begin(doc, strkey, -1, &child); 
      table_to_QSRecord(&child,item);
      QSRecord_append_array_end(doc, &child);
      break;
    case 0: case 1: case 2: case 5: case 6: case 7: case 8: case 9: case 11:
    case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19:
      QSRecord_append_array_begin(doc, strkey, -1, &child); 
      list_to_QSRecord(&child,item); 
      QSRecord_append_array_end(doc, &child);
      break;
    default: 
      if ((item->t >= 100) && (item->t <= 111)) {
        binary = b9(-1,item);
	      QSRecord_append_binary(doc, strkey, -1, QSRecord_SUBTYPE_USER, kG(binary), binary->n);
      } else {
        QSRecord_append_utf8 (doc, strkey, -1, "#UNDEFINED#", -1);
      }
  }
}

QSRecord_t* dict_to_QSRecord_new(K dict) { QSRecord_t *doc;doc=QSRecord_new();dict_to_QSRecord(doc,dict);return doc; }

void dict_to_QSRecord(QSRecord_t* doc, K dict) {
  K keys, vals, cols, data; QSRecord_t child; int i,j,rc; QSRecord_oid_t oid;
  keys = kK(dict)[0];
  vals = kK(dict)[1];
  if (11!=keys->t) { 
    QSRecord_append_array_begin(doc, "$k", 2, &child); 
    list_to_QSRecord(&child,keys);
    QSRecord_append_array_end(doc, &child);
    QSRecord_append_array_begin(doc, "$v", 2, &child); 
    list_to_QSRecord(&child,vals);
    QSRecord_append_array_end(doc, &child);
  }
  else {
    switch (vals->t) {
      case  0: for(i=0;i<keys->n;i++){ append_item(doc, kS(keys)[i], kK(vals)[i]); } ; break;
      case  1: for(i=0;i<keys->n;i++){ QSRecord_append_bool (doc, kS(keys)[i], -1, !0==kG(vals)[i]); } ; break;
      case  2: for(i=0;i<keys->n;i++){ 
        if (is_oid((char*)kU(vals)[i].g)) {
          QSRecord_oid_init_from_data(&oid,&kU(vals)[i].g[4]);
          QSRecord_append_oid(doc, kS(keys)[i], -1, &oid);
	} else {
          QSRecord_append_binary (doc, kS(keys)[i], -1, QSRecord_SUBTYPE_UUID,kU(vals)[i].g,sizeof(U)); 
	} ; 
      } ; break;
      case  4: for(i=0;i<keys->n;i++){ QSRecord_append_int32(doc, kS(keys)[i], -1, kG(vals)[i]); } ; break;
      case  5: for(i=0;i<keys->n;i++){ QSRecord_append_int32(doc, kS(keys)[i], -1, kH(vals)[i]); } ; break;
      case  6: for(i=0;i<keys->n;i++){ QSRecord_append_int32(doc, kS(keys)[i], -1, kI(vals)[i]); } ; break;
      case  7: for(i=0;i<keys->n;i++){ QSRecord_append_int64(doc, kS(keys)[i], -1, kJ(vals)[i]); } ; break;
      case  8: for(i=0;i<keys->n;i++){ QSRecord_append_double(doc, kS(keys)[i], -1, kE(vals)[i]); } ; break;
      case  9: for(i=0;i<keys->n;i++){ QSRecord_append_double(doc, kS(keys)[i], -1, kF(vals)[i]); } ; break;
      case 10: for(i=0;i<keys->n;i++){ QSRecord_append_utf8(doc, kS(keys)[i], -1, &kC(vals)[i], 1); } ; break;
      case 11: for(i=0;i<keys->n;i++){ QSRecord_append_utf8(doc, kS(keys)[i], -1, kS(vals)[i], -1); } ; break;
      case 12: for(i=0;i<keys->n;i++){ QSRecord_append_date_time(doc, kS(keys)[i], -1, 946684800000+kJ(vals)[i]/1000000); } break;
      case 13: case 14: case 17: case 18: case 19:
        for(i=0;i<keys->n;i++){ QSRecord_append_int32(doc, kS(keys)[i], -1, kI(vals)[i]); } break;
      case 15: for(i=0;i<keys->n;i++){ QSRecord_append_date_time(doc, kS(keys)[i], -1, 946684800000+kF(vals)[i]*86400000); } break;
      case 16: for(i=0;i<keys->n;i++){ QSRecord_append_int64(doc, kS(keys)[i], -1, kJ(vals)[i]); } break;
      case 98:
        cols = kK(vals->k)[0];
        data = kK(vals->k)[1];
        for(i=0;i<keys->n;i++) {
          QSRecord_append_document_begin(doc, kS(keys)[i], -1, &child); 
          for(j=0;j<cols->n;j++) append_row_item(&child, kS(cols)[j], kK(data)[j], i);
          QSRecord_append_document_end(doc, &child);
        }
        break;
    }
  }
}

K dict_to_QSRecord_q(K dict) {
  QSRecord_t *doc;
  K QSRecordq;
  doc = dict_to_QSRecord_new(dict);
  QSRecordq = ktn(KG, doc->len);
  memcpy(kG(QSRecordq),QSRecord_get_data(doc),doc->len);
  QSRecord_destroy (doc);
  return QSRecordq;
}

K QSRecord_to_dict_q(K bytes) {
  QSRecord_t *doc;
  K dict;
  doc = QSRecord_new_from_data(kG(bytes), bytes->n);
  dict = QSRecord_to_dict(doc);
  QSRecord_destroy (doc);
  return dict;
}

K QSRecord_to_dict(const QSRecord_t* doc) {
  QSRecord_t child; uint32_t childlen; const uint8_t *childdata;
  QSRecord_iter_t iter;
  K keys, vals, dict;
  int type = 0;
  keys = ktn(KS,0);
  vals = ktn(0,0);
  if(!QSRecord_iter_init(&iter, doc)){ dict=xD(keys,vals); return dict; }
  while (QSRecord_iter_next (&iter)) {
    js(&keys,ss((S)QSRecord_iter_key(&iter))); 
    vals = append_QSRecord_val(&iter, vals, &type);
  }
  if (keys->n==2) {
    if((ss("$k")==kS(keys)[0]) && (ss("$v")==kS(keys)[1])) {
      dict=xD(kK(vals)[0],kK(vals)[1]);
    } else {
      dict=xD(keys,vals);
    }
  } else {
    dict=xD(keys,vals);
  }
  return dict;
}

K QSRecord_to_array(const QSRecord_t* doc) {
  QSRecord_t child; uint32_t childlen; const uint8_t *childdata;
  QSRecord_iter_t iter;
  int i;
  int type = 0;
  K v;
  U *u;
  K vals = ktn(0,0);
  if(!QSRecord_iter_init(&iter, doc)){ return vals; }
  while (QSRecord_iter_next (&iter)) {
    vals = append_QSRecord_val(&iter, vals, &type);
  }
  switch (type) {
    case  1:v=ktn(KB,vals->n);for(i=0;i<vals->n;i++)kG(v)[i]=kK(vals)[i]->g;vals=v;break;
    case  2:v=ktn(UU,vals->n);for(i=0;i<vals->n;i++){u=(U*)kK(vals)[i]->G0;kU(v)[i]=*u;};vals=v;break;
    case  9:v=ktn(KF,vals->n);for(i=0;i<vals->n;i++)kF(v)[i]=kK(vals)[i]->f;vals=v;break;
    case  6:v=ktn(KI,vals->n);for(i=0;i<vals->n;i++)kI(v)[i]=kK(vals)[i]->i;vals=v;break;
    case  7:v=ktn(KJ,vals->n);for(i=0;i<vals->n;i++)kJ(v)[i]=kK(vals)[i]->j;vals=v;break;
    case 12:v=ktn(KP,vals->n);for(i=0;i<vals->n;i++)kJ(v)[i]=kK(vals)[i]->j;vals=v;break;
  }
  return vals;
}

K append_QSRecord_val(const QSRecord_iter_t *iter, K vals, int* type) {
  QSRecord_t child; uint32_t childlen; const uint8_t *childdata; QSRecord_subtype_t subtype;
  K bytes;
  bool f = (vals->n == 0);
  switch (QSRecord_iter_type (iter)) {
    case QSRecord_TYPE_OID:  jk(&vals,ku(oid_to_guid(QSRecord_iter_oid(iter)))); break;
    case QSRecord_TYPE_DOUBLE: *type=f?9:9==*type?9:0  ;jk(&vals,kf(QSRecord_iter_double(iter))); break;
    case QSRecord_TYPE_UTF8: jk(&vals,kp((S)QSRecord_iter_utf8(iter,NULL))); break;
    case QSRecord_TYPE_DOCUMENT:
      QSRecord_iter_document(iter, &childlen, &childdata);
      QSRecord_init_static (&child, childdata, childlen);
      jk(&vals,QSRecord_to_dict(&child));
      break;
    case QSRecord_TYPE_ARRAY:
      QSRecord_iter_array(iter, &childlen, &childdata);
      QSRecord_init_static (&child, childdata, childlen);
      jk(&vals,QSRecord_to_array(&child));
      break;
    case QSRecord_TYPE_INT32: *type=f?6:6==*type?6:0; jk(&vals,ki(QSRecord_iter_int32(iter))); break;
    case QSRecord_TYPE_INT64: *type=f?7:7==*type?7:0; jk(&vals,kj(QSRecord_iter_int64(iter))); break;
    case QSRecord_TYPE_BINARY:
      QSRecord_iter_binary(iter, &subtype, &childlen, &childdata);
      bytes = ktn(KG,childlen);
      memcpy(kG(bytes),childdata,childlen);
      switch (subtype) {
        case QSRecord_SUBTYPE_USER: jk(&vals,d9(bytes)); break;
	case QSRecord_SUBTYPE_UUID: *type=f?2:2==*type?2:0; jk(&vals,ku(*kU(bytes))); break;
	case QSRecord_SUBTYPE_BINARY:
	default:
          jk(&vals,bytes);
      }
      break;
    case QSRecord_TYPE_BOOL: *type=f?1:1==*type?1:0; jk(&vals,kb((int)QSRecord_iter_bool(iter))); break;
    case QSRecord_TYPE_DATE_TIME: *type=f?12:12==*type?12:0; jk(&vals,ktj(-KP,1000000*(QSRecord_iter_date_time(iter)-946684800000))); break;
    default: jk(&vals,kp("#UNDEFINED#")); break;
  }
  return vals;
}

K QSData_init(K host, K port, K qdatabase) {
   char *uri;
   QSDatac_init ();
   uri = QSRecord_strdup_printf ("QSDatadb://%s:%d/%s?ssl=%s", host->s, port->i, qdatabase->s, "false");
   if (!(client = QSDatac_client_new (uri))) {
    	QSDatac_cleanup ();
      krr("Invalid connection details");
      return (K)0;
   } 
   database = qdatabase->s;   
   initialised = true;
   return (K)0;
}

K QSData_cleanup(K x) {
   QSDatac_client_destroy (client);
   QSDatac_cleanup ();
   initialised = false;
   return (K)0;
}

K QSData_delete(K qtable, K qquery) {
  QSDatac_collection_t *collection;
  QSRecord_error_t error;
  QSRecord_t *doc;
  if ((qquery->t != 99) || (kK(qquery)[0]->t !=11)) { krr("type"); return (K)0; }
  collection = QSDatac_client_get_collection (client, database, qtable->s);
  doc = dict_to_QSRecord_new(qquery);
  QSDatac_collection_remove (collection, QSDataC_REMOVE_NONE, doc, NULL, &error);
  QSRecord_destroy(doc);
  QSDatac_collection_destroy (collection);
  return (K)0;
}

K QSData_drop(K qtable) {
  QSDatac_collection_t *collection;
  QSRecord_error_t error;
  collection = QSDatac_client_get_collection (client, database, qtable->s);
  QSDatac_collection_drop (collection, &error);
  QSDatac_collection_destroy (collection);
  return (K)0;
}

K QSData_add_index(K qtable, K qquery) {
  QSDatac_collection_t *collection;
  QSRecord_error_t error;
  QSRecord_t *doc;
  collection = QSDatac_client_get_collection (client, database, qtable->s);
  if ((qquery->t != 99) || (kK(qquery)[0]->t !=11)) { krr("type"); return (K)0; }
  doc = dict_to_QSRecord_new(qquery);
  if (!QSDatac_collection_create_index (collection, doc, NULL, &error)) krr(error.message);
  QSRecord_destroy(doc);
  QSDatac_collection_destroy (collection);
  return (K)0;
}

K QSData_insert(K table, K records) {
  QSDatac_collection_t *collection; QSDatac_bulk_operation_t *bulk;
  QSRecord_error_t error; QSRecord_oid_t oid; QSRecord_t reply, *doc;
  bool ret; int i,j,rc; K qoids, cols, data; 
  if (records->t != 98) { krr("type"); return (K)0; }
  collection = QSDatac_client_get_collection (client, database, table->s);
  bulk = QSDatac_collection_create_bulk_operation (collection, true, NULL);
  cols = kK(records->k)[0];
  data = kK(records->k)[1];
  rc = kK(data)[0]->n; 
  qoids = ktn(UU, rc);
  for(i=0;i<rc;i++) {
    doc = QSRecord_new();
    QSRecord_oid_init (&oid, NULL);
    QSRecord_append_oid(doc, "_id", -1, &oid);
    for(j=0;j<cols->n;j++) append_row_item(doc, kS(cols)[j], kK(data)[j], i);
    QSDatac_bulk_operation_insert (bulk, doc);
    QSRecord_destroy (doc);
    kU(qoids)[i] = oid_to_guid(&oid);
  }
  if (!QSDatac_bulk_operation_execute (bulk, &reply, &error)) { krr(error.message); return (K)0; }
  QSRecord_destroy (&reply); 
  QSDatac_bulk_operation_destroy (bulk); QSDatac_collection_destroy (collection);
  return qoids;
}

K QSData_find(K qtable, K qquery, K qfields) {
   QSDatac_collection_t *collection;
   QSDatac_cursor_t *cursor;
   const QSRecord_t *item;
   QSRecord_error_t error;
   QSRecord_t *query, *fields;
   K rtn;
   if((qtable->t!=-11)||(qfields->t!=99)||(qquery->t!=99)){krr("type");return (K)0;}; 
   collection = QSDatac_client_get_collection (client, database, qtable->s);
   query = dict_to_QSRecord_new(qquery);
   fields = dict_to_QSRecord_new(qfields);
   cursor = QSDatac_collection_find (collection, QSDataC_QUERY_NONE, 0, 0, 0, query, fields, NULL);
   rtn = ktn(0, 0);
   while (QSDatac_cursor_next (cursor, &item)) jk(&rtn,QSRecord_to_dict(item)); 
   if (QSDatac_cursor_error (cursor, &error)) { krr(error.message); return(K)0; }
   QSDatac_cursor_destroy (cursor); QSDatac_collection_destroy (collection);
   QSRecord_destroy (query); QSRecord_destroy (fields);
   return rtn;
}
