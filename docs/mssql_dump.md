# SQL Server Schema Export (T-SQL)

## Using sqlcmd

```bash
sqlcmd -S localhost -d mydb -U username -P password -i schema.sql -o output.txt
```

## Generate CREATE Scripts via SSMS

1. Open SQL Server Management Studio
2. Right-click database → Tasks → Generate Scripts
3. Select "Select specific database objects" → choose tables
4. Advanced → Types of data to script = Schema only
5. Save to new query window or file

## PowerShell with dbatools

```powershell
# Install dbatools first: Install-Module dbatools
Export-DbaSchema -SqlInstance localhost -Database mydb -Path ./schema
```

## T-SQL Script to Extract Schema

```sql
-- Get all tables
SELECT TABLE_SCHEMA, TABLE_NAME
FROM INFORMATION_SCHEMA.TABLES
WHERE TABLE_TYPE = 'BASE TABLE';

-- Get columns for a table
SELECT COLUMN_NAME, DATA_TYPE, CHARACTER_MAXIMUM_LENGTH, IS_NULLABLE
FROM INFORMATION_SCHEMA.COLUMNS
WHERE TABLE_NAME = 'YourTable'
ORDER BY ORDINAL_POSITION;

-- Get foreign keys
SELECT
    fk.name AS FK_NAME,
    tp.name AS PARENT_TABLE,
    cp.name AS PARENT_COL,
    tr.name AS REFERENCED_TABLE,
    cr.name AS REFERENCED_COL
FROM sys.foreign_keys fk
INNER JOIN sys.tables tp ON fk.parent_object_id = tp.object_id
INNER JOIN sys.tables tr ON fk.referenced_object_id = tr.object_id
INNER JOIN sys.foreign_key_columns fkc ON fk.object_id = fkc.constraint_object_id
INNER JOIN sys.columns cp ON fkc.parent_column_id = cp.column_id AND fkc.parent_object_id = cp.object_id
INNER JOIN sys.columns cr ON fkc.referenced_column_id = cr.column_id AND fkc.referenced_object_id = cr.object_id;
```

## Generate Script for All Tables

```sql
USE mydb;
GO

DECLARE @sql NVARCHAR(MAX) = '';

SELECT @sql = @sql + 'EXEC sp_executesql N''' +
    REPLACE(REPLACE(sc.text, '''', ''''''), 'CREATE TABLE ', 'CREATE TABLE dbo.') + ''''
FROM (
    SELECT TOP 100 PERCENT TEXT AS text
    FROM syscomments
    INNER JOIN sysobjects ON syscomments.id = sysobjects.id
    WHERE sysobjects.type = 'U'
    ORDER BY sysobjects.name
) AS sc;

PRINT @sql;
```

## Verify Export

```bash
grep -c "CREATE TABLE" schema.sql
grep -c "ALTER TABLE" schema.sql
```
