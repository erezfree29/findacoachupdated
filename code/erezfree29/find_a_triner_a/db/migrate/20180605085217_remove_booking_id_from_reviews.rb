class RemoveBookingIdFromReviews < ActiveRecord::Migration[5.2]
  def change
    remove_column :reviews, :booking_id
  end
end
