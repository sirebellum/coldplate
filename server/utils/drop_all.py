from sqlalchemy import create_engine, MetaData
from sqlalchemy.exc import IntegrityError
from sqlalchemy.sql import text

# Define database URI (adjust as necessary)
DATABASE_URI = 'postgresql://ir_user:securepassword@192.168.69.9:5432/ir_image_db'

def truncate_all_tables():
    # Create an engine and connect to the database
    engine = create_engine(DATABASE_URI)
    connection = engine.connect()

    # Reflect the database to get all table names
    metadata = MetaData()
    metadata.reflect(bind=engine)

    try:
        # Disable transaction to avoid issues with truncate cascades
        connection.execute(text("BEGIN;"))
        
        # Iterate over all tables and truncate them with CASCADE
        for table in reversed(metadata.sorted_tables):
            print(f"Truncating table {table.name}...")
            try:
                connection.execute(text(f"TRUNCATE TABLE {table.name} CASCADE;"))
            except IntegrityError as e:
                print(f"Failed to truncate {table.name} due to integrity constraints: {e}")

        # Commit changes
        connection.execute(text("COMMIT;"))
        print("All tables truncated successfully.")
    except Exception as e:
        print(f"An error occurred: {e}")
        connection.execute(text("ROLLBACK;"))
    finally:
        connection.close()

if __name__ == "__main__":
    truncate_all_tables()
