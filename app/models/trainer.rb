class Trainer < ApplicationRecord
  belongs_to :user
  has_many :booking
  has_many :reviews, through: :bookings
  validates :hourly_rate, numericality: { greater_than_or_equal_to: 1 }
  mount_uploader :photo, PhotoUploader
end

