from datetime import datetime
from sqlalchemy import create_engine, Column, Integer, String, DateTime, JSON, ForeignKey, Boolean
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker

# Define database URI (adjust to match your remote PostgreSQL setup)
DATABASE_URI = 'postgresql://ir_user:securepassword@192.168.69.9:5432/ir_image_db'

# Create an engine and base
engine = create_engine(DATABASE_URI)
Base = declarative_base()

# Define Image model
class ImageRecord(Base):
    __tablename__ = 'image_record'
    id = Column(Integer, primary_key=True)
    file_path = Column(String, unique=True, nullable=False)
    upload_time = Column(DateTime, default=datetime.utcnow)

# Define ProcessedData model
class ProcessedData(Base):
    __tablename__ = 'processed_data'
    id = Column(Integer, primary_key=True)
    image_id = Column(Integer, ForeignKey('image_record.id'), nullable=False)
    cluster_id = Column(Integer)
    centroid = Column(JSON)  # Optional: store centroid coordinates
    encoded_vector = Column(JSON)  # For storing CNN autoencoder results

# Create a sessionmaker
Session = sessionmaker(bind=engine)

# Function to initialize the database tables
def initialize_database():
    # Create tables if they don't exist
    Base.metadata.create_all(engine)
    print("Database tables created successfully.")

if __name__ == "__main__":
    initialize_database()
