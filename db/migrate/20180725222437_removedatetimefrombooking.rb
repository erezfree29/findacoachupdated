class Removedatetimefrombooking < ActiveRecord::Migration[5.2]
  def change

   remove_column :bookings, :time_of_booking

  end
end
