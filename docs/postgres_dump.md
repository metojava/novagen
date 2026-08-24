# PostgreSQL Schema Export

## Full Schema Export

```bash
pg_dump -U username -h localhost --schema-only -d mydb > schema.sql
```

## With INSERT Statements (for data)

```bash
pg_dump -U username -h localhost --data-only -d mydb > data.sql
```

## Schema Only with Comments

```bash
pg_dump -U username -h localhost --schema-only --verbose -d mydb > schema.sql
```

## Using psql

```bash
psql -U username -h localhost -d mydb -c "\dt"  # list tables
pg_dump -U username -h localhost --schema-only -d mydb > schema.sql
```

## Custom Format (for pgAdmin)

```bash
pg_dump -U username -h localhost -Fc mydb > schema.dump
```

## Verify Export

```bash
grep -c "CREATE TABLE" schema.sql
grep -c "ALTER TABLE" schema.sql
```
