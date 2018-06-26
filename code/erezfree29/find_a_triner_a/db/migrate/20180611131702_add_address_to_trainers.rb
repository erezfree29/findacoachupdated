class AddAddressToTrainers < ActiveRecord::Migration[5.2]
  def change
    add_column :trainers, :address, :string
  end
end
