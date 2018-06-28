class AddTimeToBookings < ActiveRecord::Migration[5.2]
  def change
    add_column :bookings, :time_of_booking, :datetime
  end
end
