class Trainer < ApplicationRecord
  belongs_to :user
  belongs_to :expertise
  has_many :bookings, dependent: :destroy
  has_many :reviews, through: :bookings
  validates :hourly_rate, numericality: { greater_than_or_equal_to: 1 }
  validates :user_id,:expertise_id,:address,:address2,:address3,:photo,:first_name,:last_name,:gender, presence: true
  mount_uploader :photo, PhotoUploader
  geocoded_by :address
  after_validation :geocode, if: :will_save_change_to_address?

end
