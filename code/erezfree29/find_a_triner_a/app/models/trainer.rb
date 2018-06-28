class Trainer < ApplicationRecord
  belongs_to :user
  belongs_to :expertise
  has_many :bookings, dependent: :destroy
  has_many :reviews, through: :bookings
  validates :hourly_rate, numericality: { greater_than_or_equal_to: 1 }
  mount_uploader :photo, PhotoUploader
end

