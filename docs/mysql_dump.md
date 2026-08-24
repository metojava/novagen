# MySQL Schema Export

## Full Schema Export

```bash
mysqldump -u username -p --no-data --databases mydb > schema.sql
```

## Single Table Schema

```bash
mysqldump -u username -p --no-data mydb table_name > table.sql
```

## With Foreign Keys Only

```bash
mysqldump -u username -p --no-data --single-transaction mydb | grep -E 'CREATE|FOREIGN|REFERENCES' > schema.sql
```

## Using MySQL Workbench

1. Open MySQL Workbench
2. Connect to database
3. Menu: Database → Reverse Engineer
4. Follow wizard to select schema
5. File → Export → Forward Engineer SQL CREATE Script

## Verify Export

```bash
grep -c "CREATE TABLE" schema.sql
```
