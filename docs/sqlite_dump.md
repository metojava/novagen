# SQLite Schema Export

## Using sqlite3 CLI

```bash
sqlite3 mydb.db ".schema" > schema.sql
```

## Schema Only (no data)

```bash
sqlite3 mydb.db ".schema --noschema"  # omit to include schema
```

## Specific Tables

```bash
sqlite3 mydb.db ".schema users orders products" > tables.sql
```

## Full Dump with Data

```bash
sqlite3 mydb.db ".dump" > full_dump.sql
```

## Using Python

```python
import sqlite3

conn = sqlite3.connect('mydb.db')
cursor = conn.cursor()

# Get all tables
cursor.execute("SELECT name FROM sqlite_master WHERE type='table'")
tables = cursor.fetchall()

with open('schema.sql', 'w') as f:
    for table in tables:
        # Get CREATE statement
        cursor.execute(f"SELECT sql FROM sqlite_master WHERE name='{table[0]}'")
        create = cursor.fetchone()[0]
        f.write(create + ';\n\n')

conn.close()
```

## Verify Export

```bash
grep -c "CREATE TABLE" schema.sql
```
