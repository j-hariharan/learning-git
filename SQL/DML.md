# SQL DML

**Example table:** Table - ADMIN
<br>
| Field | Type | Null | Key | Default | Extra |
| ----- | ----------- | ---- | --- | ------- | ----- |
| ADNO | int(11) | NO | PRI | NULL | |
| NAME | varchar(20) | YES | | pending | |
| CLASS | char(3) | YES | | NULL | |
| BUS | int(11) | YES | MUL | NULL | |

<br><br>

## ADD RECORDS

<br>

### all fields

---

```sql
insert into ADMIN values(5, "Akash", "12B", 15);
```

**mysql> select \* from ADMIN;**

| ADNO | NAME  | CLASS | BUS |
| ---- | ----- | ----- | --- |
| 5    | Akash | 12B   | 15  |

_Note: since BUS field is a foreign key, to insert this record, a bus of ROUTE 15 should exist in the parent table (TRANSPORT)._

<br>

### specific fields

---

```sql
insert into ADMIN(ADNO, BUS) values(4, 20);
```

**mysql> select \* from ADMIN;**

| ADNO | NAME    | CLASS | BUS |
| ---- | ------- | ----- | --- |
| 4    | pending | NULL  | 20  |
| 5    | Akash   | 12B   | 15  |

_Note: primary keys and fields with NOT NULL constraints cannot be omitted._

<br><br>

## UPDATE RECORDS

```sql
update ADMIN
set NAME = "Anil"
where ADNO = 4;
```

**mysql> select \* from ADMIN;**

| ADNO | NAME  | CLASS | BUS |
| ---- | ----- | ----- | --- |
| 4    | Anil  | NULL  | 20  |
| 5    | Akash | 12B   | 15  |

_Note: Any condition can be given in the WHERE clause. If more than one record satisfy the condition, all of them will be updated in a similar way._

<br>

```sql
update ADMIN
set NAME = "Anup", CLASS = "12A"
where BUS = 20;
```

**mysql> select \* from ADMIN;**

| ADNO | NAME  | CLASS | BUS |
| ---- | ----- | ----- | --- |
| 4    | Anup  | 12A   | 20  |
| 5    | Akash | 12B   | 15  |

_Note: Expressions involving values of other fields of the record and any operations can also be used as the new value in SET clause. Update to NULL by setting FIELD=NULL in SET CLAUSE (without double-quotes around NULL)._

<br><br>

## DELETING RECORDS

```sql
delete from ADMIN
where CLASS="12A";
```

**mysql> select \* from ADMIN;**
| ADNO | NAME | CLASS | BUS |
|------|-------|-------|------|
| 5 | Akash | 12B | 15 |

_Note: To delete all contents of the table, exclude the WHERE clause._
