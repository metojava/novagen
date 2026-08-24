# NovaGen Documentation

## Quick Links

| Database | Guide |
|----------|-------|
| MySQL | [mysql_dump.md](mysql_dump.md) |
| PostgreSQL | [postgres_dump.md](postgres_dump.md) |
| SQLite | [sqlite_dump.md](sqlite_dump.md) |
| SQL Server (T-SQL) | [mssql_dump.md](mssql_dump.md) |
| Oracle (PL/SQL) | [oracle_dump.md](oracle_dump.md) |

## Database Schema Export Guide

NovaGen generates code from SQL DDL (CREATE TABLE statements). To use NovaGen with an existing database, you first need to export its schema.

### MySQL

```bash
mysqldump -u username -p --no-data --databases mydb > schema.sql
```

### PostgreSQL

```bash
pg_dump -U username -h localhost --schema-only -d mydb > schema.sql
```

### SQLite

```bash
sqlite3 mydb.db ".schema" > schema.sql
```

### SQL Server (T-SQL)

Use SQL Server Management Studio:
1. Right-click database → Tasks → Generate Scripts
2. Select tables → Advanced → Types of data to script = Schema only

Or with sqlcmd:
```bash
sqlcmd -S localhost -d mydb -U username -P password -i schema.sql
```

### Oracle (PL/SQL)

```sql
SELECT DBMS_METADATA.GET_DDL('TABLE', table_name)
FROM user_tables;
```

Or use expdp:
```bash
expdp username/password FULL=Y CONTENT=metadata_only DUMPFILE=schema.dmp
```

## Using Exported Schema with NovaGen

Once you have your schema.sql file:

```bash
./novagen --input schema.sql --framework flask --project myapi
```

See [index.html](../index.html) for full NovaGen documentation.
