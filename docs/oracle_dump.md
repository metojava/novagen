# Oracle Schema Export (PL/SQL)

## Using DBMS_METADATA

```sql
-- Get DDL for all tables
SELECT DBMS_METADATA.GET_DDL('TABLE', table_name)
FROM user_tables;

-- Get DDL for specific table
SELECT DBMS_METADATA.GET_DDL('TABLE', 'EMPLOYEES')
FROM DUAL;

-- Get all schema objects
SELECT DBMS_METADATA.GET_DDL(object_type, object_name)
FROM user_objects
WHERE object_type IN ('TABLE', 'VIEW', 'SEQUENCE', 'PROCEDURE', 'FUNCTION', 'PACKAGE');
```

## Using expdp (Data Pump Export)

```bash
expdp username/password@oracle_service FULL=Y CONTENT=metadata_only DIRECTORY=data_pump_dir DUMPFILE=schema.dmp
```

## Using exp Utility

```bash
exp username/password@oracle_service FULL=y ROWS=n FILE=schema.dmp
```

## Using SQLcl or SQL*Plus

```bash
sqlplus username/password@localhost:1521/service_name @export_schema.sql
```

## export_schema.sql Script

```sql
SET LINESIZE 32767
SET PAGESIZE 0
SET FEEDBACK OFF
SET VERIFY OFF
SET TERMOUT OFF
SPOOL schema.sql

-- Tables
SELECT DBMS_METADATA.GET_DDL('TABLE', table_name) FROM user_tables;

-- Indexes
SELECT DBMS_METADATA.GET_DDL('INDEX', index_name) FROM user_indexes WHERE index_name NOT IN (SELECT constraint_name FROM user_constraints);

-- Sequences
SELECT DBMS_METADATA.GET_DDL('SEQUENCE', sequence_name) FROM user_sequences;

-- Views
SELECT DBMS_METADATA.GET_DDL('VIEW', view_name) FROM user_views;

-- Foreign Keys
SELECT DBMS_METADATA.GET_DDL('REF_CONSTRAINT', constraint_name)
FROM user_constraints
WHERE constraint_type = 'R';

SPOOL OFF
SET TERMOUT ON
SET FEEDBACK ON
SET VERIFY ON
```

## Get Column Details

```sql
SELECT
    table_name,
    column_name,
    data_type,
    data_length,
    data_precision,
    data_scale,
    nullable,
    data_default
FROM user_tab_columns
WHERE table_name = 'YOUR_TABLE'
ORDER BY column_id;
```

## Get Foreign Keys

```sql
SELECT
    a.constraint_name,
    a.table_name,
    b.column_name,
    c.table_name AS ref_table,
    d.column_name AS ref_column
FROM user_constraints a
JOIN user_cons_columns b ON a.constraint_name = b.constraint_name
JOIN user_constraints c ON a.r_constraint_name = c.constraint_name
JOIN user_cons_columns d ON c.constraint_name = d.constraint_name
WHERE a.constraint_type = 'R';
```

## Verify Export

```bash
grep -c "CREATE TABLE" schema.sql
grep -c "ALTER TABLE" schema.sql
```
