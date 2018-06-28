class RemoveSomethingFromBooking < ActiveRecord::Migration[5.2]
  def change

    remove_column :bookings, :review_id
  end
end
